#ifndef TAPDEVICE_H
#define TAPDEVICE_H

#include <fcntl.h>
#include <linux/if.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "common.h"
#include "ethernet.h"

namespace TapDevice {

class TapDevice {
private:
  int fd;
  ifreq ifr;
  uint8_t ipv4[4];
  uint8_t mac[5];
  error setFlags(short flags);
  std::tuple<Ethernet::Packet, error> readPacket();
  error writePacket(Ethernet::Packet pkt, std::vector<uint8_t> rsp);

public:
  ~TapDevice() { close(this->fd); }
  error Init();
  void HandleRequests();
  void SetIPv4Address(uint8_t p0, uint8_t p1, uint8_t p2, uint8_t p3);
  void SetHardwareAddress(uint8_t p0, uint8_t p1, uint8_t p2, uint8_t p3,
                          uint8_t p4, uint8_t p5);
};

std::tuple<TapDevice *, error> New();

} // namespace TapDevice

#endif
