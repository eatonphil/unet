#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <string>
#include <tuple>

typedef int error;

#define ok 0
#define notok -1

inline void log(std::string msg) { std::cout << msg << std::endl; }

inline void fatal(std::string msg, error err) {
  std::cout << msg << std::endl;
  exit(err);
}

#endif
