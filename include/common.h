#ifndef COMMON_H
#define COMMON_H

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include <sys/types.h>

#define ok 0
#define notok -1

#define MIN(a, b) (a < b ? a : b)

#define COPY(a, s, b)                                                          \
  for (int i = 0; i < int(s); ++i)                                             \
    a[i] = b[i];

typedef int error;

inline void log(std::string msg) { std::cout << msg << std::endl; }

inline void fatal(std::string msg, error err) {
  std::cout << msg << std::endl;
  exit(err);
}

#endif
