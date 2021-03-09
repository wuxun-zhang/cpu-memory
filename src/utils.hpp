// Utility functions

#ifndef UTILS_HPP
#define UTILS_HPP

#include <sys/time.h>

double ms_now() {
  struct timeval time;
  gettimeofday(&time, nullptr);
  return (1e+3 * time.tv_sec + 1e-3 * time.tv_usec);
}

#endif
