#include <cstring>
#include <netinet/in.h>

#include "ethernet.h"

using namespace std;
using namespace Ethernet;

typedef struct __attribute__((packed)) {
  unsigned char destinationMac[6];
  unsigned char sourceMac[6];
  char16_t header;
  unsigned char payload[];
} rawPacket;

char16_t Packet::GetType() { return this->type; }

// TODO: Pull out frame check sequence
void Packet::Parse(char buffer[MFU], ssize_t maxReadLength) {
  rawPacket *pkt = (rawPacket *)buffer;
  memcpy(this->destinationMac, pkt->destinationMac,
         sizeof(this->destinationMac));
  memcpy(this->sourceMac, pkt->sourceMac, sizeof(this->sourceMac));
  this->type = htons(pkt->header);
  memcpy(this->payload, pkt->payload, maxReadLength - 6 + 6 + 4);
}
