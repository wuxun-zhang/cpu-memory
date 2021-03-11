#include <iostream>
#include <vector>

#include "utils.hpp"

#define NUM_ITERS (200)
#define NUM_ELEM (16 * 1024 * 1024)
#define STEP (16)

using DTYPE = float;

void run_v1(std::vector<DTYPE> &src) {
    double start = ms_now();
    // iterations
    for (size_t i = 0; i < static_cast<size_t>(NUM_ITERS); ++i) {
        for (size_t n = 0; n < static_cast<size_t>(NUM_ELEM); ++n) {
            src[n] += 1.1;
        }
    }
    double elapsed = ms_now() - start;
    std::cout << "elapsed time of v1: " << (elapsed / NUM_ITERS) << " msec\n";
}

// the only difference is in v2 there will be a write operation every STEP elements
void run_v2(std::vector<DTYPE> &src) {
    double start = ms_now();
    // iterations
    for (size_t i = 0; i < static_cast<size_t>(NUM_ITERS); ++i) {
        for (size_t n = 0; n < static_cast<size_t>(NUM_ELEM); n += STEP) {
            src[n] += 1.1;
        }
    }
    double elapsed = ms_now() - start;
    std::cout << "elapsed time of v2: " << (elapsed / NUM_ITERS) << " msec\n";
}

int main() {
    std::vector<DTYPE> src_1(NUM_ELEM, 1);
    std::vector<DTYPE> src_2(NUM_ELEM, 2);

    run_v1(src_1);
    run_v2(src_2);
    
    return 1;
}