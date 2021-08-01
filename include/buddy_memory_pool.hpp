#ifndef BUDDY_MEMORY_POOL_HPP
#define BUDDY_MEMORY_POOL_HPP

#include <cmath>
#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>

// Manages memory in power of two increments
// * If 2^(U-1)<S<=2^U: Allocate the whole block
// * Else: Recursively divide the block equally and test the condition at each
// *      time, when it satisfies, allocate the block and get out the loop.
//
// Take-aways
//  ** little external fragmentations
//  ** much internal fragmentations - fixed allocation size
//  ** can be implemented by binary tree, search complexity is O(logN)
//  ** easier to coalescing
namespace buddy_mempool {

// minimal allocation unit - 4 bytes
#define MIN_ALLOC_UNIT (4)

class MemoryPool {
private:
    // default ctor - need initial capacity
    MemoryPool(size_t n) {
        // allocate real buffer
        std::unique_ptr<char> temp_mem = std::make_unique<char>(n);
        data_ = temp_mem.release();

        int num_elem_in_free_list
                = static_cast<int>(std::ceil(log(n) / log(2)));
        global_vars_.size = num_elem_in_free_list + 1;

        for (int i = 0; i <= num_elem_in_free_list; ++i)
            global_vars_.free_list[i].clear();

        global_vars_.free_list[num_elem_in_free_list].push_back({0, n - 1});
    }

    // default d-ctor
    ~MemoryPool() { release(); }

public:
    // Singletone
    static MemoryPool &Instance() {
        static MemoryPool inst(2048);
        return inst;
    }

    // Divide process
    // time complexity is O(log(N))
    void *allocate(size_t n) {
        int chunk_idx = static_cast<int>(ceil(log(n) / log(2)));
        std::pair<int64_t, size_t> temp;
        if (global_vars_.free_list[chunk_idx].size() > 0) {
            temp = global_vars_.free_list[chunk_idx][0];
            // remove chunks from free list
            global_vars_.free_list[chunk_idx].erase(
                    global_vars_.free_list[chunk_idx].begin());
            global_vars_.addr_to_size_map[temp.first]
                    = temp.second - temp.first + 1;

            std::cout << "Memory from " << temp.first << " to " << temp.second
                      << " allocated"
                      << "\n";
        } else {
            // search upwards to find larger chunks
            int i = chunk_idx + 1;
            for (; i < global_vars_.size; ++i) {
                if (global_vars_.free_list[i].size() > 0) break;
            }
            if (i == global_vars_.size) {
                std::cout << "Failed to allocate memory since no proper chunks "
                             "found\n";
            } else { // found
                // (0, 127)
                temp = global_vars_.free_list[i][0];
                global_vars_.free_list[i].erase(
                        global_vars_.free_list[i].begin());
                i--;
                // divide block into two smaller halfs
                // ** initial state -> {(0, 127)}
                // ** first loop -> {(0, 63), (64, 127)}, {}
                // ** second loop -> {(0, 31), (32, 63)}, {(64, 127)}
                // ...
                for (; i >= chunk_idx; --i) {
                    std::pair<int64_t, size_t> pair1, pair2;
                    pair1 = std::make_pair(temp.first,
                            temp.first + (temp.second - temp.first) / 2);
                    pair2 = std::make_pair(
                            temp.first + (temp.second + 1 - temp.first) / 2,
                            temp.second);
                    global_vars_.free_list[i].push_back(pair1);
                    global_vars_.free_list[i].push_back(pair2);

                    temp = global_vars_.free_list[i][0];

                    global_vars_.free_list[i].erase(
                            global_vars_.free_list[i].begin());
                }
                global_vars_.addr_to_size_map[temp.first]
                        = temp.second - temp.first + 1;
                std::cout << "Memory from " << temp.first << " to "
                          << temp.second << " allocated"
                          << "\n";
            }
        }
        return nullptr; /*static_cast<void *>(data_ + temp.first)*/
    }

    // Combine process
    void deallocate(/*void *ptr*/ int64_t base_addr) {
        if (global_vars_.addr_to_size_map.find(base_addr)
                == global_vars_.addr_to_size_map.end()) {
            std::cout << "Invalid free request\n";
            return;
        }

        size_t found_size = global_vars_.addr_to_size_map[base_addr];
        int chunk_idx = static_cast<int>(ceil(log(found_size) / log(2)));

        int i, buddyNumber, buddyAddress;
        global_vars_.free_list[chunk_idx].push_back(
                {base_addr, base_addr + pow(2, chunk_idx) - 1});

        buddyNumber = base_addr / found_size;
        if (buddyNumber % 2 != 0)
            buddyAddress = base_addr - pow(2, chunk_idx);
        else
            buddyAddress = base_addr + pow(2, chunk_idx);

        for (i = 0; i < global_vars_.free_list[chunk_idx].size(); ++i) {
            if (global_vars_.free_list[chunk_idx][i].first == buddyAddress) {
                // merge two smaller halfs into one bigger one
                if (buddyNumber % 2 == 0)
                    global_vars_.free_list[chunk_idx + 1].push_back({base_addr,
                            2 * (pow(2, chunk_idx) - 1) + base_addr});
                else
                    global_vars_.free_list[chunk_idx + 1].push_back(
                            {buddyAddress,
                                    2 * pow(2, chunk_idx) + buddyAddress});

                // then delete two buddy chunks in lower level
                global_vars_.free_list[chunk_idx].erase(
                        global_vars_.free_list[chunk_idx].begin() + i);
                global_vars_.free_list[chunk_idx].erase(
                        global_vars_.free_list[chunk_idx].begin()
                        + global_vars_.free_list[chunk_idx].size() - 1);
                break;
            }
        }
        global_vars_.addr_to_size_map.erase(base_addr);
        std::cout << "Returning Memory " << base_addr << " to pool\n";
    }

    // free all of memory chunks in the memory pool
    void release() {
        if (data_) {
            delete[] data_;
            std::cout << "Releasing memory buffer...\n";
        }
    }

private:
    struct GlobalVars {
        // how many level in this memory pool - depends on the initial capacity
        // of memory pool
        size_t size;
        // free list to store pairs containing starting and ending
        // address/position
        std::vector<std::pair<int64_t, size_t>> free_list[10000];
        // mapping from starting address to size - for deallocation
        // quickly to search the given pointer
        std::unordered_map<int64_t, size_t> addr_to_size_map;
    } global_vars_;

    // real buffer
    char *data_ {nullptr};
};

} // namespace buddy_mempool

#endif
