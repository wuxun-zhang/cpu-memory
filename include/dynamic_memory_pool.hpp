#ifndef DYNAMIC_MEMORY_POOL_HPP_
#define DYNAMIC_MEMORY_POOL_HPP_

#include <iostream>
#include <memory>
#include <vector>

namespace dynamic_mempool {

// Memory pool with dynamic management
// Implemented by two list
// * allocated_list_ -> list of memory chunks being allocated
// * free_list_ -> list of memory chunks being unused in increasing order
//
// Take-aways
//  ** linear search complexity
//  ** poor data locality - not contiguous between adjacent nodes
class MemoryPool {
private:
    // default ctor
    MemoryPool() {
        Entry e;
        e.data = nullptr;
        e.size = 0;
        // init list
        free_list_.push_back(e);
        allocated_list_.push_back(e);
    }

    // default d-ctor
    ~MemoryPool() { release(); }

public:
    // Singleton instance
    static MemoryPool &Instance() {
        static MemoryPool inst;
        return inst;
    }

    // allocate a memory chunk of size n
    // add to allocated_list_
    // search time complexity: O(N)
    void *allocate(size_t n, size_t alignment = 64) {
        // make sure alignment
        n = (n + alignment - 1) / alignment * alignment;
        if (0 == n) n = alignment;
        Entry e;
        e.data = nullptr;
        e.size = 0;
        int delete_idx = -1;
        // no proper memory size
        if (free_list_.back().size < n) {
            std::unique_ptr<char> mem_ptr = std::make_unique<char>(n);
            e.data = mem_ptr.release();
            e.size = n;
        } else {
            for (int i = free_list_.size(); i >= 0; --i) {
                if (free_list_[i].size < n) {
                    delete_idx = i + 1; // found
                    break;
                }
            }
            e.data = free_list_[delete_idx].data;
            e.size = n;
            free_list_.erase(free_list_.begin() + delete_idx);
        }

        allocated_list_.push_back(e);
        std::cout << "Allocating " << e.size << " bytes from memory pool!\n";
        return e.data;
    }

    // deallocate a memory chunk pointed by ptr
    // move from allocated_list_ to free_list_
    void deallocate(void *ptr) {
        int delete_idx = -1;
        size_t mem_size = 0;
        Entry e;
        e.data = nullptr;
        e.size = 0;
        // fast path
        if (allocated_list_.back().data == ptr) {
            e = allocated_list_.back();
            allocated_list_.pop_back();
        } else {
            for (int i = 0; i < allocated_list_.size(); ++i) {
                if (ptr == allocated_list_[i].data) {
                    delete_idx = i;
                    e = allocated_list_[i];
                    break;
                }
            }
            // remove from allocated_list_
            if (delete_idx == -1) {
                // the given ptr is not managed by this memory pool
            } else {
                allocated_list_.erase(allocated_list_.begin() + delete_idx);
            }
        }

        // add into free_list_
        if (free_list_.back().size < e.size) {
            free_list_.push_back(e);
        } else if (free_list_.size() == 2) {
            free_list_.push_back(free_list_.back());
            free_list_[1] = e;
        } else {
            int insert_idx = 0;
            // search for position to be inserted
            for (int i = 0; i < free_list_.size(); ++i) {
                if (free_list_[i].size >= e.size) {
                    insert_idx = i;
                    break;
                }
                free_list_.insert(free_list_.begin() + insert_idx, e);
            }
        }
        std::cout << "Returning " << e.size << " bytes to memory pool!\n";
    }

    // free all resources listed in free_list_ and allocated_list_
    void release() {
        for (auto &item : allocated_list_) {
            if (!item.data) {
                char *ptr = static_cast<char *>(item.data);
                delete[] ptr;
                item.data = nullptr;
                item.size = 0;
            }
        }

        for (auto &item : free_list_) {
            if (!item.data) {
                char *ptr = static_cast<char *>(item.data);
                delete[] ptr;
                item.data = nullptr;
                item.size = 0;
            }
        }

        allocated_list_.clear();
        free_list_.clear();
        std::cout << "Release all resources in the memory pool!\n";
    }

private:
    struct Entry {
        // pointer to memory chunk
        void *data;
        // size of allocated memory chunk
        size_t size;
    };

    // list of allocated memory chunks
    std::vector<Entry> allocated_list_;
    // list of free memory chunks, sorted by increasing order
    std::vector<Entry> free_list_;
};

} // namespace dynamic_mempool
#endif
