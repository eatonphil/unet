#include <arpa/inet.h>
#include <inttypes.h>

#include "arp.h"

using namespace std;

typedef struct __attribute__((packed)) {
  uint16_t hardwareType; // e.g. Ethernet/WiFi
  uint16_t protocolType;
  uint8_t hardwareAddressLength;
  uint8_t protocolAddressLength;
  uint16_t opcode;
  // Hard-coded for Ethernet/IEEE 802 MAC address
  uint8_t senderHardwareAddress[6];
  // Hard-coded for IPv4 (4 bytes)
  uint32_t senderProtocolAddress;
  // Hard-coded for Ethernet/IEEE 802 MAC address
  uint8_t targetHardwareAddress[6];
  // Hard-coded for IPv4 (4 bytes)
  uint32_t targetProtocolAddress;
} rawPacket;

tuple<vector<uint8_t>, ssize_t, error>
ARP::HandleRequest(Ethernet::Packet ethernetPkt, const string address,
                   const uint8_t mac[6]) {
  vector<uint8_t> response;

  // TODO: this is dynamic if not hard-coded to Ethernet style and IPv4
  ssize_t reqSize = sizeof(rawPacket);

  // Read request into rawPacket
  vector<uint8_t> payload = ethernetPkt.GetPayload();
  rawPacket *pkt = (rawPacket *)(&payload[0]);

  // Make copy of request for response
  rawPacket rsp = *pkt;

  error err;
  switch (htons(pkt->opcode)) {
  case OPCODE_ARP_REQUEST:
    // Set self hardware address
    memcpy(rsp.senderHardwareAddress, mac, 6);

    // Set self protocol address
    err = inet_pton(AF_INET, address.c_str(), &rsp.senderProtocolAddress);
    if (err == 0) {
      // Invalid address:
      // http://man7.org/linux/man-pages/man3/inet_pton.3.html#RETURN_VALUE
      return {response, reqSize, notok};
    }

    // Set dest mac address
    memcpy(rsp.targetHardwareAddress, pkt->senderHardwareAddress, 6);
    // Set dest protocol address
    rsp.targetProtocolAddress = pkt->senderProtocolAddress;
    break;
  default:
    // TODO: implement RARP
    return {response, reqSize, notok};
  }

  rsp.opcode = htons(OPCODE_ARP_RESPONSE);

  response.assign((uint8_t *)&rsp, (uint8_t *)&rsp + reqSize);
  return {response, reqSize, ok};
}
