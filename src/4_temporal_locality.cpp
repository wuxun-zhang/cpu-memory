// This is an example to demonstrate the impact of temporal locality on MatMul's
// performance

#include <iostream>
#include <vector>

#include "utils.hpp"

/* Code to clear cache */
/* This computer has 12288K L3 cache, which is 3072K ints */
#define ASIZE ((1 << 21) + (1 << 20))
static int stuff[ASIZE];
static int sink;

static void clear_cache() {
    int x = sink;
    for (int i = 0; i < ASIZE; i += 16 /*cache line size: 64bytes*/)
        x += stuff[i];
    sink = x;
}

using DTYPE = float;

// This size should be:
//  - not too large, make sure (2 * BLOCK_SIZE + BLOCK_SIZE^2) is smaller than
//      the size of L1 cache
//  - not too small, make full use of L1 cache and improve temporal locality
//      as much as possible
#define BLOCK_SIZE (64)

class block_matmul {
private:
    // dimensions
    int n_;
    // arrays
    std::vector<DTYPE> A_;
    std::vector<DTYPE> B_;
    std::vector<DTYPE> C_;
    // total number of iterations for the three loops
    int iters_;

    void reset() {
        std::fill(C_.begin(), C_.end(), 0);
    }

    // pack sub-block of B_ into a matrix that is small enough to be
    // stored into L1 cache
    void packBMatrix(DTYPE *dst, DTYPE *src) {
        for (int i = 0; i < BLOCK_SIZE; ++i) {
            for (int j = 0; j < BLOCK_SIZE; ++j) {
                *(dst + i * BLOCK_SIZE + j) = *(src + i * n_ + j);
            }
        }
    }

public:
    block_matmul(int n) : n_(n) {
        iters_ = n_ * n_ * n_;
        A_ = std::vector<DTYPE>(n_ * n_, 1);
        B_ = std::vector<DTYPE>(n_ * n_, 1);
        C_ = std::vector<DTYPE>(n_ * n_, 0);
    }

    // loop K -> loop N -> loop M
    void knm() {
        reset();
        clear_cache();
        DTYPE packed_B[BLOCK_SIZE * BLOCK_SIZE];
        double start = ms_now();
        for (int k = 0; k < n_; k += BLOCK_SIZE) {
            for (int j = 0; j < n_; j += BLOCK_SIZE) {
                // pack B
                packBMatrix(&packed_B[0], &B_[k * n_ + j]);
                for (int i = 0; i < n_; ++i) {
                    for (int kk = k; kk < k + BLOCK_SIZE; ++kk) {
                        float tmp = A_[i * n_ + kk];
                        for (int jj = j; jj < j + BLOCK_SIZE; ++jj) {
                            C_[i * n_ + jj] += tmp
                                    * packed_B[(kk - k) * BLOCK_SIZE
                                            + (jj - j)];
                        }
                    }
                }
            }
        }
        double elapsed = ms_now() - start;
        printf("version: block-knm, latency: %.2f ms, "
            "time per innermost iter: %.2f ns\n", elapsed,
                (elapsed / (double)iters_) * 1e6);
        fflush(stdout);
    }

};

int main() {
    block_matmul block_mm(1024);
    block_mm.knm();
    return 0;
}
