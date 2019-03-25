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

error maccpy(uint8_t buffer[6], string mac) {
  if (sscanf(mac.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &buffer[5],
             &buffer[4], &buffer[3], &buffer[2], &buffer[1], &buffer[0]) != 6) {
    return notok;
  }

  return ok;
}

tuple<vector<uint8_t>, ssize_t, error>
ARP::HandleRequest(shared_ptr<Ethernet::Packet> ethernetPkt, string address,
                   string mac) {
  vector<uint8_t> response;

  // TODO: this is dynamic if not hard-coded to Ethernet style and IPv4
  ssize_t reqSize = sizeof(rawPacket);

  // Read request into rawPacket
  vector<uint8_t> payload = ethernetPkt->GetPayload();
  rawPacket *pkt = (rawPacket *)(&payload[0]);
  pkt->opcode = htons(pkt->opcode);

  // Make copy of request for response
  rawPacket rsp = *pkt;

  error err;
  if (pkt->opcode == OPCODE_ARP_REQUEST) {
    rsp.opcode = ntohs(OPCODE_ARP_RESPONSE);

    // Set self mac address
    err = maccpy(rsp.senderHardwareAddress, mac);
    if (err == notok) {
      return {response, reqSize, err};
    }

    // Set self protocol address
    err = inet_pton(AF_INET, address.c_str(), &rsp.senderProtocolAddress);
    if (err == 0) {
      // Invalid address:
      // http://man7.org/linux/man-pages/man3/inet_pton.3.html#RETURN_VALUE
      return {response, reqSize, notok};
    }

    // Set dest mac address
    COPY(rsp.targetHardwareAddress, sizeof(rsp.targetHardwareAddress),
         pkt->senderHardwareAddress)
    // Set dest protocol address
    rsp.targetProtocolAddress = pkt->senderProtocolAddress;
  } else {
    // TODO: implement RARP
    return {response, reqSize, notok};
  }

  response.assign((uint8_t *)&rsp, (uint8_t *)&rsp + reqSize);
  return {response, reqSize, ok};
}
