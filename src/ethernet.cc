#include <netinet/in.h>

#include "ethernet.h"

using namespace std;

typedef struct __attribute__((packed)) {
  uint8_t destinationMac[6];
  uint8_t sourceMac[6];
  uint16_t header;
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

uint16_t Ethernet::Packet::GetType() { return this->type; }

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

  memcpy(this->payload, payload.data(), size);

  this->packetSize = size + OVERHEAD;

  return size;
}

void Ethernet::Packet::Deserialize(const uint8_t buffer[MFU],
                                   ssize_t packetSize) {
  const rawPacket *pkt = (rawPacket *)buffer;
  memcpy(this->destinationMac, pkt->destinationMac,
         sizeof(this->destinationMac));
  memcpy(this->sourceMac, pkt->sourceMac, sizeof(this->sourceMac));
  this->type = htons(pkt->header);
  this->packetSize = packetSize;
  memcpy(this->payload, pkt->payload, packetSize - OVERHEAD);
}

ssize_t Ethernet::Packet::Serialize(uint8_t buffer[MFU]) {
  const ssize_t s = this->packetSize;
  rawPacket pkt = {};

  // Copy internal data to pkt
  pkt.header = ntohs(this->type);
  // Destination is now the source of the original packet
  memcpy(pkt.destinationMac, this->sourceMac, 6);
  // Source is now the current device's mac address
  memcpy(pkt.sourceMac, this->destinationMac, 6);
  memcpy(pkt.payload, this->payload, s - OVERHEAD);

  // Copy into output buffer
  memcpy(buffer, &pkt, s);

  // Write CRC to buffer
  uint32_t crc = htons(CRC(buffer, s - Ethernet::CRC_SIZE));
  uint8_t crcBytes[4];
  memcpy(crcBytes, &crc, Ethernet::CRC_SIZE);
  memcpy(buffer + s - Ethernet::CRC_SIZE, crcBytes, Ethernet::CRC_SIZE);

  return s;
}

// TODO: validate CRC
error Ethernet::Validate(Packet pkt, ssize_t payloadSize) { return ok; }
