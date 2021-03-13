#include <iostream>
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
#define NUM_ELEM (32 * 1024 * 1024)
#define STEP (16)

using DTYPE = float;

void run_v1(std::vector<DTYPE> &src) {
    double total_time = 0.;
    // iterations
    for (size_t i = 0; i < static_cast<size_t>(NUM_ITERS); ++i) {
        clear_cache();
        double start = ms_now();
        for (size_t n = 0; n < static_cast<size_t>(NUM_ELEM); ++n) {
            src[n] += 1.1f;
        }
        total_time += (ms_now() - start);
    }
    std::cout << "Time per iteration of v1: " <<
        (total_time / NUM_ITERS) << " msec\n";
}

// the only difference is in v2 there will be a write operation every STEP elements
void run_v2(std::vector<DTYPE> &src) {
    double total_time = 0.;
    // iterations
    for (size_t i = 0; i < static_cast<size_t>(NUM_ITERS); ++i) {
        clear_cache();
        double start = ms_now();
        for (size_t n = 0; n < static_cast<size_t>(NUM_ELEM); n += STEP) {
            src[n] += 1.1f;
        }
        total_time += (ms_now() - start);
    }
    std::cout << "Time per iteration of v2: " <<
        (total_time / NUM_ITERS) << " msec\n";
}

int main() {
    std::vector<DTYPE> src_1(NUM_ELEM, 1);
    std::vector<DTYPE> src_2(NUM_ELEM, 2);

    run_v1(src_1);
    run_v2(src_2);
    
    return 1;
}