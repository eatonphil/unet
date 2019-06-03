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

tuple<Ethernet::Packet, error> TapDevice::TapDevice::readPacket() {
  Ethernet::Packet pkt;
  uint8_t buffer[Ethernet::MFU];
  ssize_t c = read(this->fd, buffer, sizeof(buffer));
  if (c == notok) {
    return {pkt, errno};
  }

  pkt.Deserialize(buffer, c);

  return {pkt, ok};
}

error TapDevice::TapDevice::writePacket(Ethernet::Packet pkt,
                                        vector<uint8_t> rsp) {
  uint8_t buffer[Ethernet::MFU];
  size_t payloadSize = 0, totalSize = 0;
  while (rsp.size()) {
    payloadSize = pkt.SetPayload(rsp);

    totalSize = pkt.Serialize(buffer);
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

TapDevice::TapDevice::HandleRequests() {
  Ethernet::Packet pkt;
  std::vector<uint8_t> rsp;
  ssize_t reqSize;
  while (true) {
    auto t = this->readPacket();
    pkt = std::move(std::get<0>(t));
    err = std::get<1>(t);
    if (err != ok) {
      fatal("Failed while reading packet", err);
    }

    switch (pkt.GetType()) {
    case Ethernet::IPv4: {
      log("[INFO] Handling IPv4 packet");
      auto handled = IPV4::HandleRequest(pkt);
      rsp = std::get<0>(handled);
      reqSize = std::get<1>(handled);
      err = std::get<2>(handled);
      if (err != ok) {
        fatal("Error while handling IPv4 packet", err);
      }
      break;
    }
    default:
      log("[INFO] Unrecognized packet type");
      continue;
    }

    err = Ethernet::Validate(pkt, reqSize);
    if (err != ok) {
      log("[INFO] Invalid packet");
      continue;
    }

    err = this->writePacket(pkt, mac, rsp);
    if (err != ok) {
      fatal("Error while writing packet", err);
    }
  }
}

TapDevice::TapDevice::SetIPv4Address(uint8_t p0, uint8_t p1, uint8_t p2,
                                     uint8_t p3) {
  this->ipv4[0] = p0;
  this->ipv4[1] = p1;
  this->ipv4[2] = p2;
  this->ipv4[3] = p3;
}

TapDevice::TapDevice::SetHardwareAddress(uint8_t p0, uint8_t p1, uint8_t p2,
                                         uint8_t p3, uint8_t p4, uint8_t p5) {
  this->mac[0] = p0;
  this->mac[1] = p1;
  this->mac[2] = p2;
  this->mac[3] = p3;
  this->mac[4] = p4;
  this->mac[5] = p5;
}
