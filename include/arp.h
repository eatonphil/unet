#ifndef ARP_H
#define ARP_H

#include "common.h"
#include "ethernet.h"

namespace ARP {
std::tuple<std::vector<uint8_t>, ssize_t, error>
HandleRequest(Ethernet::Packet pkt, std::string address, std::string mac);

const int OPCODE_ARP_REQUEST = 1;
const int OPCODE_ARP_RESPONSE = 2;

const int HRD_ETHERNET = 1;
const int HRD_IEEE802 = 6;
} // namespace ARP

#endif
