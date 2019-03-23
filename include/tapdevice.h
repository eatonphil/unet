#ifndef TAPDEVICE_H
#define TAPDEVICE_H

#include <string>

#include "common.h"
#include "ethernet.h"

namespace TapDevice {
class TapDevice {
private
  int fd;
  string ifname;
  buffer unsigned char[Ethernet::MAX_FRAME_LENGTH];

  error setFlags(short flags);
  error addRemoveRoute();

public
  TapDevice(int fd, string ifname);
  ~TapDevice();
  error SetUp();
  std::tuple<ssize_t, error> Read(buf char *, n size_t);
  error RemoveRoute(string address);
  error AddRoute(string address);
}

std::tuple<TapDevice *, error>
New();

} // namespace TapDevice

#endif
