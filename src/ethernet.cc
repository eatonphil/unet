using namespace std;
using namespace Ethernet;

typedef struct __attribute__((packed)) {
  unsigned char destinationMac[6];
  unsigned char sourceMac[6];
  uint16_t header;
  unsigned char payload[];
} rawPacket;

Packet *Packet::Packet(unsigned char destinationMac[6],
                       unsigned char sourceMac[6], uint16_t type,
                       unsigned char payload[]) {
  this.destinationMac = destinationMac;
  this.sourceMac = sourceMac;
  this.type = type;
  this.payload = payload;
};

uint16_t Packet::GetType() { return this.type; }

// TODO: Pull out frame check sequence
Packet *Parse(const unsigned char buffer[MAX_FRAME_LENGTH], int maxReadLength) {
  struct rawPacket *pkt = (struct rawPacket *)buffer;
  return new Packet(pkt->destinationMac, pkt->sourceMac, htons(pkt->header),
                    pkt->payload);
}
