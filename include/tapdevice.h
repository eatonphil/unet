#ifndef TAPDEVICE_H
#define TAPDEVICE_H

#include <fcntl.h>
#include <linux/if.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "common.h"
#include "ethernet.h"

namespace TapDevice {
class TapDevice {
private:
  int fd;
  ifreq ifr;
  char buffer[Ethernet::MFU];

  error setFlags(short flags);

public:
  ~TapDevice() { close(this->fd); }
  error Init();
  error ReadPacket(Ethernet::Packet &pkt);
  error SetIP(std::string address);
};

std::tuple<TapDevice *, error> New();

} // namespace TapDevice

#endif
