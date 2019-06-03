#include <arpa/inet.h>
#include <inttypes.h>

#include "ipv4.h"

using namespace std;

typedef struct __attribute__((packed)) {
  uint8_t version : 4;
  uint8_t headerLength : 4;
  // Differentiated services code point (e.g. VoIP)
  uint8_t dsCode : 6;
  uint8_t networkCongestion : 2;
  // Total length of packet including header and data
  uint16_t totalLength;
  // Not sure what this is for
  uint16_t identification;
  uint16_t fragmentFlags : 3;
  uint16_t fragmentOffset : 13;
  uint8_t ttl : 4;
  uint8_t protocol : 4;
  uint16_t headerChecksum;
  uint32_t sourceIP;
  uint32_t destinationIP;
  // This may be zero or up to 16 bytes of data
  uint8_t options[16];
} rawHeader;

tuple<vector<uint8_t>, ssize_t, error>
IPv4::HandleRequest(Ethernet::Packet ethernetPkt) {
  vector<uint8_t> response;

  // TODO: this is dynamic if not hard-coded to Ethernet style and IPv4
  ssize_t reqSize = sizeof(rawHeader);

  // Read request into rawHeader
  vector<uint8_t> payload = ethernetPkt.GetPayload();
  rawHeader *pkt = (rawHeader *)(&payload[0]);

  // Make copy of request for response
  rawHeader rsp = *pkt;

  error err;
  switch (htons(pkt->opcode)) {
  case OPCODE_IP_REQUEST:
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
    return {response, reqSize, notok};
  }

  rsp.opcode = htons(OPCODE_IP_RESPONSE);

  response.assign((uint8_t *)&rsp, (uint8_t *)&rsp + reqSize);
  return {response, reqSize, ok};
}
