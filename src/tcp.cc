#include "tcp.h"

using namespace std;

typedef struct __attribute__((packed)) {
  uint16_t sourcePort;
  uint16_t destPort;
  uint32_t sourceNumber;
  uint32_t ackNumber;
  uint16_t dataOffset : 4;
  uint16_t flags : 12;
  uint16_t windowSize;
  uint16_t checksum;
  uint16_t urgentPointer;
  // Zero to 40 bytes of options.
  uint8_t options[40];
} rawHeader;

tuple<vector<uint8_t>, ssize_t, error>
TCP::Handler::HandleRequest(const Network::Packet &networkPkt) {
  vector<uint8_t> response;

  ssize_t reqSize = sizeof(rawHeader);

  // Read request into rawHeader
  vector<uint8_t> payload = networkPkt.GetPayload();
  rawHeader *pkt = (rawHeader *)(&payload[0]);

  // Make copy of request for response
  rawHeader rsp = *pkt;

  auto connectionId =
      make_pair(pkt->sourcePort, pkt->destPort, networkPkt.sourceAddress,
                networkPkt.destAddress);
  auto connectionIt = this->connections.find(connectionId);
  if (connectionIt == this->connections.end()) {
    // New connection, initialize
    this->connections[connectionId] = new TCP::Connection;
    if (pkt->sourceNumber && !pkt->ackNumber) {
      rsp.sourceNumber++;
      rsp.ackNumber = 12;
      return {response, reqSize, ok};
    } else {
    }
  }

  response.assign((uint8_t *)&rsp, (uint8_t *)&rsp + reqSize);
  return {response, reqSize, ok};
}

TCP::Handler::AddHandler(uint16_t port, void (*handler)(TCP::Connection &)) {
  this->handlers[port] = handler;
}
