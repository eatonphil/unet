#ifndef IPV4_H
#define IPV4_H

#include "common.h"
#include "layers.h"
#include "tcp.h"

namespace IPv4 {

const int OPCODE_IPV4_REQUEST = 1;
const int OPCODE_IPV4_RESPONSE = 2;

const int HRD_ETHERNET = 1;
const int HRD_IEEE802 = 6;

class Handler {
private:
  uint8_t address[4];
  TCP::Handler &handler;

public:
  std::tuple<std::vector<uint8_t>, ssize_t, error>
  HandlePacket(Layers::HardwarePacket pkt);
}

} // namespace IPv4

#endif
