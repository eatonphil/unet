#ifndef ETHERNET_H
#define ETHERNET_H

#include <linux/if_ether.h>

#include "common.h"

namespace Ethernet {
const ssize_t CRC_SIZE = 4;
// (6byte dest + 6byte src + 2byte type + 4byte CRC) + 46-1500bytes of data.
const ssize_t OVERHEAD = 18;
const ssize_t MINTU = 64 - OVERHEAD;
const ssize_t MTU = 1500;
const ssize_t MFU = MTU + OVERHEAD;

const int ARP = ETH_P_ARP;
const int IP = ETH_P_IP;

class Packet {
private:
  ssize_t packetSize;
  uint8_t destinationMac[6];
  uint8_t sourceMac[6];
  char16_t type;
  uint32_t frameCheck;
  uint8_t payload[MTU];

public:
  char16_t GetType();
  std::vector<uint8_t> GetPayload();
  ssize_t SetPayload(std::vector<uint8_t>);
  void Deserialize(const uint8_t buffer[MFU], ssize_t n);
  ssize_t Serialize(uint8_t buffer[MFU]);
};

error Validate(std::shared_ptr<Packet> pkt, ssize_t payloadSize);
} // namespace Ethernet

#endif
