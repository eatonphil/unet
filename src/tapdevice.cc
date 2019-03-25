#include <arpa/inet.h>
#include <cstring>
#include <linux/if_tun.h>
#include <linux/route.h>
#include <sys/socket.h>

#include "tapdevice.h"

using namespace std;

error TapDevice::TapDevice::setFlags(short flags) {
  int sock = socket(AF_INET, SOCK_DGRAM, 0);

  // Get current flags
  error err = ioctl(sock, SIOCGIFFLAGS, &ifr);
  if (err == -1) {
    return errno;
  }

  ifr.ifr_flags |= flags;

  // Commit new flags
  err = ioctl(sock, SIOCSIFFLAGS, &ifr);
  close(sock);
  if (err == -1) {
    return errno;
  }

  return ok;
}

tuple<shared_ptr<Ethernet::Packet>, error> TapDevice::TapDevice::ReadPacket() {
  shared_ptr<Ethernet::Packet> pkt = make_shared<Ethernet::Packet>();
  uint8_t buffer[Ethernet::MFU];
  ssize_t c = read(this->fd, buffer, sizeof(buffer));
  if (c == notok) {
    return {pkt, errno};
  }

  pkt->Deserialize(buffer, c);

  return {pkt, ok};
}

error TapDevice::TapDevice::WritePacket(shared_ptr<Ethernet::Packet> pkt,
                                        vector<uint8_t> rsp) {
  uint8_t buffer[Ethernet::MFU];
  size_t payloadSize = 0, totalSize = 0;
  while (rsp.size()) {
    payloadSize = pkt->SetPayload(rsp);

    totalSize = pkt->Serialize(buffer);
    ssize_t c = write(this->fd, buffer, totalSize);
    if (c == notok) {
      return errno;
    }

    rsp.erase(rsp.begin(), rsp.begin() + MIN(rsp.size(), payloadSize));
  }
  return ok;
}

error TapDevice::TapDevice::Init() {
  this->fd = open("/dev/net/tun", O_RDWR);
  if (this->fd == notok) {
    return errno;
  }

  // Ethernet layer with no additional packet information
  memset(&this->ifr, 0, sizeof(this->ifr));
  this->ifr.ifr_flags = IFF_TAP | IFF_NO_PI;

  error err = ioctl(this->fd, TUNSETIFF, &this->ifr);
  if (err == notok) {
    return errno;
  }

  // Set up and running
  this->setFlags(IFF_UP | IFF_RUNNING);

  return ok;
}
