// This is to benchmark the impact of different levels of cache on the
// performance CPU doing operations.
// This can also be used to get the approximate size of caches (e.g L1, L2 or L3)

#include <iostream>
#include <vector>

#include "utils.hpp"

using DTYPE = float;

// L1 cache
#define KB1 (1 << 10)
#define KB2 (1 << 11)
#define KB4 (1 << 12)
#define KB8 (1 << 13)
#define KB16 (1 << 14)
#define KB32 (1 << 15)
// L2 cache
#define KB64 (1 << 16)
#define KB96 ((1 << 16) + (1 << 15))
#define KB128 (1 << 17)
#define KB160 ((1 << 17) + (1 << 15))
#define KB256 (1 << 18)
// L3 cache
#define KB512 (1 << 19)
#define KB1024 (1 << 20)
#define KB2048 (1 << 21)
#define KB4096 (1 << 22)
#define KB8192 (1 << 23)
#define KB12288 ((1 << 23) + (1 << 22))
// RAM
#define KB16384 (1 << 24)
#define KB32768 (1 << 25)

// computer clock (Hz)
// queried from `lscpu`
#define CLOCK (3600 * 1e6)

int main() {
    // define size of working set
    std::vector<size_t> working_set {/*L1*/KB1, KB2, KB4, KB8, KB16, KB32,
        /*L2*/KB64, /*KB96,*/ KB128, /*KB160,*/ KB256, /*L3*/KB512, KB1024, KB2048,
        KB4096, KB8192, KB12288, /*RAM*/KB16384, KB32768};

    for (auto ws : working_set) {
        size_t num_elem = ws / sizeof(DTYPE);
        std::vector<DTYPE> data(num_elem, 1.);
        size_t len_mod = num_elem - 1;
        size_t steps = 64 * 1024 * 1024;
        double start = ms_now();
        for (int j = 0; j < steps; ++j) {
            data[(j * 16) & len_mod]++;
        }
        double end = ms_now();
        double time_per_element = (end - start) / (double)steps;
        // memory size in KiB
        double total_size = (double)(num_elem * sizeof(DTYPE)) /
            (/*1KiB = */1024.0);
        double cycles_per_element = (time_per_element / 1000.0) *
            ((double)/*clock = */CLOCK);
        printf("Working set size: %.2f KiB,clear cycles per element is %.4f \n",
            total_size, cycles_per_element);
    }
    return 1;
}
