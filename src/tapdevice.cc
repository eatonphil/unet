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

error TapDevice::TapDevice::SetIP(string address) {
  // Set up address
  struct sockaddr_in addr = {};
  addr.sin_family = AF_INET;
  addr.sin_addr = {};
  inet_pton(AF_INET, address.c_str(), &addr.sin_addr.s_addr);

  // Add to request
  memcpy(&this->ifr.ifr_addr, &addr, sizeof(struct sockaddr));

  int sock = socket(AF_INET, SOCK_DGRAM, 0);

  // Add or remove the IP
  error err = ioctl(sock, SIOCSIFADDR, &this->ifr);
  close(sock);
  if (err == -1) {
    return errno;
  }

  return ok;
}

error TapDevice::TapDevice::ReadPacket(Ethernet::Packet &pkt) {
  ssize_t c = read(this->fd, this->buffer, Ethernet::MFU);
  if (c == notok) {
    return errno;
  }

  pkt.Parse(this->buffer, c);

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

  log(this->ifr.ifr_name);

  return ok;
}
