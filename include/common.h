#ifndef COMMON_H
#define COMMON_H

#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

#include <sys/types.h>

#define ok 0
#define notok -1

#define MIN(a, b) (a < b ? a : b)

typedef int error;

inline void log(std::string msg) { std::cout << msg << std::endl; }

inline void log_ipv4(uint32_t spa) {
  std::cout << (spa >> 24 & 0xff) << "." << (spa >> 16 & 0xff) << "."
            << (spa >> 8 & 0xff) << "." << (spa & 0xff) << "" << std::endl;
}

inline void log_mac(const uint8_t mac[6]) {
  for (unsigned int i = 0; i < 6; i++) {
    std::cout << std::hex << std::setw(2) << std::setfill('0') << int(mac[i]);
    if (i < 5) {
      std::cout << ":";
    }
  }
  std::cout << std::endl;
}

inline void fatal(std::string msg, error err) {
  std::cout << msg << std::endl;
  exit(err);
}

#endif
