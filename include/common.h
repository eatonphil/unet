#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <string>
#include <tuple>

typedef int error;

#define ok 0

void fatal(std::string msg, error err) {
  std::cout << msg << std::endl;
  exit(err);
}

#endif
