// This is to approximately calculate the size of a cache line
#include <iostream>
#include <algorithm>
#include <vector>

#include "utils.hpp"

/* Code to clear cache */
/* This computer has 12288K L3 cache, which is 3072K ints */
#define ASIZE ((1 << 21) + (1 << 20))
static int stuff[ASIZE];
static int sink;

static void clear_cache()
{
  int x = sink;
  for (int i = 0; i < ASIZE; i += 16/*cache line size: 64bytes*/)
    x += stuff[i];
  sink = x;
}

#define NUM_ITERS (200)
#define NUM_ELEM (16 * 1024 * 1024)

using DTYPE = float;

void run(std::vector<DTYPE> &src) {
    // different skipping steps
    std::vector<size_t> steps {1, 2, 4, 8, 16, 32, 64, 128, 256};
    // std::vector<size_t> steps {16};
    for (size_t s : steps) {
        double total_time = 0.;
        // iterations
        for (size_t i = 0; i < static_cast<size_t>(NUM_ITERS); ++i) {
            clear_cache();
            double start = ms_now();
            for (size_t n = 0; n < static_cast<size_t>(NUM_ELEM); n += s) {
                src[n] += 1.1;
            }
            total_time += (ms_now() - start);
        }
        std::cout << "Step: " << (s * sizeof(DTYPE)) << "bytes, time cost: "
            << (total_time / NUM_ITERS) << " msec\n";
    }
}

int main() {
    std::vector<DTYPE> data(NUM_ELEM, 1);
    run(data);
    return 1;
}