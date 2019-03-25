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

  error setFlags(short flags);

public:
  ~TapDevice() { close(this->fd); }
  error Init();
  std::tuple<Ethernet::Packet, error> ReadPacket();
  error WritePacket(Ethernet::Packet pkt, std::vector<uint8_t> rsp);
};

std::tuple<TapDevice *, error> New();

} // namespace TapDevice

#endif
