#ifndef LAYERS_H
#define LAYERS_H

#include "common.h"

namespace Layers {

const ssize_t CRC_SIZE = 4;
// (6byte dest + 6byte src + 2byte type + 4byte CRC) + 46-1500bytes of data.
const ssize_t OVERHEAD = 18;
const ssize_t MINTU = 64 - OVERHEAD;
const ssize_t MTU = 1500;
const ssize_t MFU = MTU + OVERHEAD;

class HardwarePacket {
public:
  uint16_t GetType();
  std::vector<uint8_t> GetPayload();
  ssize_t SetPayload(std::vector<uint8_t>);
  void Deserialize(const uint8_t buffer[MFU], ssize_t n);
  ssize_t Serialize(uint8_t buffer[MFU]);
};

class NetworkPacket {
public:
  std::vector<uint8_t> GetPayload();
  ssize_t SetPayload(std::vector<uint8_t>);
}

} // namespace Layers

#endif
