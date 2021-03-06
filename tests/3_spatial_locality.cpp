// This is an example to demonstrate the impact of spatial locality on MatMul's
// performance

#include <iostream>
#include <vector>
#include <algorithm>

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

class matmul {
private:
    // dimensions
    int n_;
    // arrays
    std::vector<DTYPE> A_;
    std::vector<DTYPE> B_;
    std::vector<DTYPE> C_;
    // total number of innermost loops
    int iters_;

    void reset() {
        std::fill(C_.begin(), C_.end(), 0);
    }

public:
    matmul(int n) : n_(n) {
        iters_ = n_ * n_ * n_;
        A_ = std::vector<DTYPE>(n_ * n_, 1);
        B_ = std::vector<DTYPE>(n_ * n_, 1);
        C_ = std::vector<DTYPE>(n_ * n_, 0);
    }

    // loop M -> loop N -> loop K
    void mnk() {
        reset();
        clear_cache();
        double start = ms_now();
        for (int i = 0; i < n_; ++i) {
            for (int j = 0; j < n_; ++j) {
                DTYPE sum = 0.;
                for (int k = 0; k < n_; ++k) {
                    sum += A_[i * n_ + k] * B_[k * n_ + j]; 
                }
                C_[i * n_ + j] = sum;
            }
        }
        double elapsed = ms_now() - start;
        printf("version: mnk, latency: %.2f ms, "
            "time per innermost iter: %.2f ns\n", elapsed,
            (elapsed / (double)iters_) * 1e6);
        fflush(stdout);
    }

    // loop N -> loop M -> loop K
    void nmk() {
        reset();
        clear_cache();
        double start = ms_now();
        for (int j = 0; j < n_; ++j) {
            for (int i = 0; i < n_; ++i) {
                DTYPE sum = 0.;
                for (int k = 0; k < n_; ++k) {
                    sum += A_[i * n_ + k] * B_[k * n_ + j]; 
                }
                C_[i * n_ + j] = sum;
            }
        }
        double elapsed = ms_now() - start;
        printf("version: nmk, latency: %.2f ms, "
            "time per innermost iter: %.2f ns\n", elapsed,
            (elapsed / (double)iters_) * 1e6);
        fflush(stdout);
    }

    // loop N -> loop K -> loop M
    void nkm() {
        reset();
        clear_cache();
        double start = ms_now();
        for (int j = 0; j < n_; ++j) {
            for (int k = 0; k < n_; ++k) {
                DTYPE tmp = B_[k * n_ + j];
                for (int i = 0; i < n_; ++i) {
                    C_[i * n_ + j] += A_[i * n_ + k] * tmp;
                }
            }
        }
        double elapsed = ms_now() - start;
        printf("version: nkm, latency: %.2f ms, "
            "time per innermost iter: %.2f ns\n", elapsed,
            (elapsed / (double)iters_) * 1e6);
        fflush(stdout);
    }

    // loop K -> loop N -> loop M
    void knm() {
        reset();
        clear_cache();
        double start = ms_now();
        for (int k = 0; k < n_; ++k) {
            for (int j = 0; j < n_; ++j) {
                DTYPE tmp = B_[k * n_ + j];
                for (int i = 0; i < n_; ++i) {
                    C_[i * n_ + j] += A_[i * n_ + k] * tmp;
                }
            }
        }
        double elapsed = ms_now() - start;
        printf("version: knm, latency: %.2f ms, "
            "time per innermost iter: %.2f ns\n", elapsed,
            (elapsed / (double)iters_) * 1e6);
        fflush(stdout);
    }

    // loop K -> loop M -> loop N
    void kmn() {
        reset();
        clear_cache();
        double start = ms_now();
        for (int k = 0; k < n_; ++k) {
            for (int i = 0; i < n_; ++i) {
                DTYPE tmp = A_[i * n_ + k];
                for (int j = 0; j < n_; ++j) {
                    C_[i * n_ + j] += tmp * B_[k * n_ + j];
                }
            }
        }
        double elapsed = ms_now() - start;
        printf("version: kmn, latency: %.2f ms, "
            "time per innermost iter: %.2f ns\n", elapsed,
            (elapsed / (double)iters_) * 1e6);
        fflush(stdout);
    }

    // loop M -> loop K -> loop N
    // best spatial locality
    void mkn() {
        reset();
        clear_cache();
        double start = ms_now();
        for (int i = 0; i < n_; ++i) {
            for (int k = 0; k < n_; ++k) {
                DTYPE tmp = A_[i * n_ + k];
                for (int j = 0; j < n_; ++j) {
                    C_[i * n_ + j] += tmp * B_[k * n_ + j];
                }
            }
        }
        double elapsed = ms_now() - start;
        printf("version: mkn, latency: %.2f ms, "
            "time per innermost iter: %.2f ns\n", elapsed,
            (elapsed / (double)iters_) * 1e6);
        fflush(stdout);
    }
};

int main() {
    // [1024x1024] x [1024x1024]
    matmul mm(1024);
    mm.mnk();
    mm.nmk();
    mm.nkm();
    mm.knm();
    mm.kmn();
    mm.mkn();
    return 0;
}