// This is to benchmark the impact of different levels of cache on the
// performance CPU doing operations.
// This can also be used to get the approximate size of caches (e.g L1, L2 or L3)

#include <iostream>
#include <random>
#include <vector>

#include "utils.hpp"

using namespace std;

using DTYPE = float;

#define MAX_NUM_ITERATIONS (10000)

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<DTYPE> dist (1.0, 2.0);

    for (int i = 0; i <= 10; ++i) {
        size_t num_elem = (1 << (i + 10));
        std::vector<DTYPE> data(num_elem);

        double start = ms_now();
        for (int idx = 0; idx < MAX_NUM_ITERATIONS; ++idx) {
            DTYPE value = dist(gen);
            for (int j = 0; j < num_elem; ++j) {
                data[j] = value;
            }
        }
        double end = ms_now();
        // second
        double time_per_iter = (end - start) / (double)MAX_NUM_ITERATIONS / 1000.0;
        // KiB
        double total_size = (double)(num_elem * sizeof(DTYPE)) / (/*1KiB = */1024.0);
        std::cout << "Number of element: " << num_elem << ", total size: "
            << total_size << "KiB, bandwidth: " <<
            ((double)(total_size) / time_per_iter / 1000.0) << "MiB/s\n";
    }
    return 1;
}

