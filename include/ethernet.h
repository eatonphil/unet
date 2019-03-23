#ifndef ETHERNET_H
#define ETHERNET_H

#include <linux/if_ether.h>

#include "common.h"

namespace Ethernet {
class Packet {
private:
  unsigned char destinationMac[6];
  unsigned char sourceMac[6];
  uint16_t type;
  unsigned char payload[];

public:
  Packet(unsigned char destinationMac[6], unsigned char sourceMac[6],
         uint16_t type, unsigned char payload[]);

  uint16_t GetType();

  void *Parse(char *buffer);
}

// (6byte dest + 6byte src + 2byte type + 4byte CRC) + 46-1500bytes of data.
int MAX_FRAME_LENGTH = 1518;

int ARP = ETH_P_ARP;
int IP = ETH_P_IP;
} // namespace Ethernet

#endif
