#ifndef TAPDEVICE_H
#define TAPDEVICE_H

#include "unistd.h"

#include "common.h"
#include "ethernet.h"

namespace TapDevice {
class TapDevice {
private:
  int fd;
  std::string ifname;
  unsigned char buffer[Ethernet::MAX_FRAME_LENGTH];

  error setFlags(short int f);
  error addRemoveRoute();

public:
  TapDevice(int fd, std::string ifname) : fd(fd), ifname(ifname){};
  ~TapDevice() { close(fd); };
  error SetUp();
  error NextPacket(Ethernet::Packet &pkt);
  error RemoveRoute(std::string address);
  error AddRoute(std::string address);
};

std::tuple<TapDevice *, error> New();

} // namespace TapDevice

#endif
