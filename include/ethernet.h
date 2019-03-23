#ifndef ETHERNET_H
#define ETHERNET_H

#include <linux/if_ether.h>
#include <stdint.h>

#include "common.h"

namespace Ethernet {
class Packet {
private:
  unsigned char destinationMac[6];
  unsigned char sourceMac[6];
  char16_t type;
  unsigned char payload[];

public:
  char16_t GetType();

  void Parse(const unsigned char *buffer, int n);
};

// (6byte dest + 6byte src + 2byte type + 4byte CRC) + 46-1500bytes of data.
const int MAX_FRAME_LENGTH = 1518;

const int ARP = ETH_P_ARP;
const int IP = ETH_P_IP;
} // namespace Ethernet

#endif
