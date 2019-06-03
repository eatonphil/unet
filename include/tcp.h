#ifndef TCP_H
#define TCP_H

#include <map>

#include "common.h"
#include "layers.h"
#include "tcp.h"

namespace TCP {

class Connection {
public:
  std::tuple<std::vector<uint8_t>, error> Read(uint16_t);
  std::tuple<error> Write(std::vector<uint8_t>);
}

class Handler {
private:
  map<uint16_t, void (*)(Connection &)> handlers;

public:
  std::tuple<std::vector<uint8_t>, ssize_t, error>
  AddHandler(uint16_t, void (*)(Connection &));
  HandlePacket(Layers::NetworkPacket);
}

} // namespace TCP

#endif
