#include <netinet/in.h>

#include "ethernet.h"

using namespace std;

typedef struct __attribute__((packed)) {
  uint8_t destinationMac[6];
  uint8_t sourceMac[6];
  char16_t header;
  uint8_t payload[Ethernet::MTU];
} rawPacket;

// CRC implementation from Hacker's Delight
// https://www.hackersdelight.org/hdcodetxt/crc.c.txt
uint32_t reverse(uint32_t x) {
  x = ((x & 0x55555555) << 1) | ((x >> 1) & 0x55555555);
  x = ((x & 0x33333333) << 2) | ((x >> 2) & 0x33333333);
  x = ((x & 0x0F0F0F0F) << 4) | ((x >> 4) & 0x0F0F0F0F);
  x = (x << 24) | ((x & 0xFF00) << 8) | ((x >> 8) & 0xFF00) | (x >> 24);
  return x;
}

uint32_t CRC(uint8_t *message, ssize_t size) {
  int i, j;
  uint32_t byte, crc;

  i = 0;
  crc = 0xFFFFFFFF;
  while (i < size) {
    byte = message[i];         // Get next byte.
    byte = reverse(byte);      // 32-bit reversal.
    for (j = 0; j <= 7; j++) { // Do eight times.
      if ((int)(crc ^ byte) < 0) {
        crc = (crc << 1) ^ 0x04C11DB7;
      } else {
        crc = crc << 1;
      }
      byte = byte << 1; // Ready next msg bit.
    }
    i++;
  }
  return reverse(~crc);
}

char16_t Ethernet::Packet::GetType() { return this->type; }

vector<uint8_t> Ethernet::Packet::GetPayload() {
  vector<uint8_t> p(this->payload, this->payload + this->packetSize - OVERHEAD);
  return p;
}

ssize_t Ethernet::Packet::SetPayload(vector<uint8_t> payload) {
  // Add padding
  while (payload.size() < MINTU) {
    payload.push_back(0);
  }

  ssize_t size = payload.size();
  if (size > MTU) {
    size = MTU;
  }

  COPY(this->payload, size, payload)

  this->packetSize = size + OVERHEAD;

  return size;
}

void Ethernet::Packet::Deserialize(const uint8_t buffer[MFU],
                                   ssize_t packetSize) {
  const rawPacket *pkt = (rawPacket *)buffer;
  COPY(this->destinationMac, sizeof(this->destinationMac), pkt->destinationMac)
  COPY(this->sourceMac, sizeof(this->sourceMac), pkt->sourceMac)
  this->type = htons(pkt->header);
  this->packetSize = packetSize;
  COPY(this->payload, packetSize - OVERHEAD, pkt->payload)
}

ssize_t Ethernet::Packet::Serialize(uint8_t buffer[MFU]) {
  const ssize_t s = this->packetSize;
  rawPacket pkt = {};

  // Copy internal data to pkt
  pkt.header = ntohs(this->type);
  COPY(pkt.destinationMac, sizeof(this->destinationMac), this->destinationMac)
  COPY(pkt.sourceMac, sizeof(this->sourceMac), this->sourceMac)
  COPY(pkt.payload, s - OVERHEAD, this->payload);

  // Copy into output buffer
  COPY(buffer, s, ((uint8_t *)&pkt));

  // Write CRC to buffer
  buffer[s - Ethernet::CRC_SIZE] = CRC(buffer, s - Ethernet::CRC_SIZE);

  return s;
}

// TODO: validate CRC
error Ethernet::Validate(Packet pkt, ssize_t payloadSize) { return ok; }
