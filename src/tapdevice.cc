#include <arpa/inet.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <unistd.h>

#include "tapdevice.h"

using namespace std;
using namespace TapDevice;

error TapDevice::setFlags(short flags) {
  error err;
  struct ifreq ifr;
  strncpy(ifr.ifr_name, this.ifname, IFNAMSIZ);

  // Get current flags
  err = ioctl(fd, SIOCGIFFLAGS, &ifr);
  if (err == -1) {
    return errno;
  }

  ifr.ifr_flags |= flags;

  // Commit new flags
  err = ioctl(fd, SIOCSIFFLAGS, &ifr);
  if (err == -1) {
    return errno;
  }

  return ok;
}

error TapDevice::SetUp() { return this.setFlags(IFF_UP | IFF_RUNNING); }

error TapDevice::addRemoveRoute(string address, int cmd) {
  error err;
  struct rtentry route;

  // Clear out any garbage on the stack
  memset(&route, 0, sizeof(route));

  // Set up address
  addr = (struct sockaddr_in *)&route.rt_dst;
  addr->sin_family = AF_INET;
  inet_pton(AF_INET, address.c_str(), &addr->sin_addr.s_addr);

  // Set up gateway
  addr = (struct sockaddr_in *)&route.rt_gateway;
  addr->sin_family = AF_INET;
  inet_pton(AF_INET, "0.0.0.0", &addr->sin_addry.s_addr);

  // Set up broadcast
  addr = (struct sockaddr_in *)&route.rt_genmask;
  addr->sin_family = AF_INET;
  inet_pton(AF_INET, "255.255.255.0", &addr->sin_addry.s_addr);

  // Set route flags
  route.rt_flags = RTF_UP | RTF_GATEWAY;
  route.rt_metric = 100;

  // Add or remove the route
  err = ioctl(this.fd, cmd, &route);
  if (err == -1) {
    return errno;
  }

  return ok;
}

error TapDevice::NextPacket(Ethernet::Packet &pkt) {
  ssize_t c = read(this.fd, this.buffer, Ethernet::MAX_FRAME_LENGTH);
  if (c == -1) {
    return errno;
  }

  pkt.Parse(buffer, c);

  return ok;
}

error TapDevice::RemoveRoute(string address) {
  return this.addRemoveRoute(address, SIOCDELRT);
}

error TapDevice::AddRoute(string address) {
  return this.addRemoveRoute(address, SIOCADDRT);
}

tuple<TapDevice *, error> New() {
  struct ifreq ifr;
  int fd;
  error err;

  fd = open("/dev/net/tun", O_RDWR);
  if (fd < 0) {
    return {0, fd};
  }

  // Ethernet layer with no additional packet information
  ifr.ifr_flags = IFF_TAP | IFF_NO_PI;

  err = ioctl(fd, TUNSETIFF, (void *)&ifr);
  if (err < 0) {
    close(fd);
    return {0, err};
  }

  string s;
  s.copy(ifr.ifr_name, 0, IFNAMSIZ);
  return {new TapDevice(fd, s), 0};
}
