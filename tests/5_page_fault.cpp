// This example aims to benchmark the impact of page fault on the effectiveness
// of accessing memory

#include <algorithm>
#include <iostream>
#include <random>
#include <vector>

#include "utils.hpp"

/* Code to clear cache */
/* This computer has 12288K L3 cache, which is 3072K ints */
#define NUM_INTS_IN_L3_CACHE ((1 << 21) + (1 << 20))

// Maximum number of steps to run
#define MAX_NUM_ITERS (64 * 1024 * 1024)

using DTYPE = float;

template <typename T>
class BenchPageFault {
private:
    std::vector<T> data_ {};
    // number of elements, a square array
    int num_elem_;
    // number of benchmark iterations
    int num_iter_;

    void reset() {
        data_ = std::vector<T>(num_elem_ * num_elem_);
        T count = static_cast<T>(10);
        std::generate(
                data_.begin(), data_.end(), [&count](void) { return count++; });
    }

    void clear_cache() {
        int array[NUM_INTS_IN_L3_CACHE];
        std::mt19937 gen;
        std::uniform_int_distribution<int> dist(0, 10);
        for (int i = 0; i < NUM_INTS_IN_L3_CACHE; ++i)
            array[i] = dist(gen);
    }

public:
    BenchPageFault(int num_elem, int num_iter)
        : num_elem_(num_elem), num_iter_(num_iter) {
        reset();
    }

    void run() {
        std::vector<size_t> step {100, 500, 2000};
        size_t len_mod = data_.size() - 1;
        for (auto s : step) {
            clear_cache();
            double start = ms_now();
            for (size_t i = 0; i < static_cast<size_t>(MAX_NUM_ITERS); ++i) {
                data_[(i * s) & len_mod]++;
            }
            double elapsed = ms_now() - start;
            printf("step size: %ld bytes, total time: %.6f msec, "
                   "latency per access: %.6f ns\n",
                    s * sizeof(T), elapsed,
                    elapsed / (double)MAX_NUM_ITERS * 1e6f);
            fflush(stdout);
        }
    }
};

int main() {
    BenchPageFault<float> bench(1024, 1);
    bench.run();
    return 0;
}
