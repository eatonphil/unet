#include "arp.h"
#include "common.h"
#include "ethernet.h"
#include "tapdevice.h"

using namespace std;

int main(int argc, char **argv) {
  TapDevice::TapDevice dev;
  error err = dev.Init();
  if (err != ok) {
    fatal("Failed to create, bring up tap device", err);
  }

  std::shared_ptr<Ethernet::Packet> pkt;
  std::vector<uint8_t> rsp;
  ssize_t reqSize;
  while (true) {
    auto t = dev.ReadPacket();
    pkt = std::move(std::get<0>(t));
    err = std::get<1>(t);
    if (err != ok) {
      fatal("Failed while reading packet", err);
    }

    switch (pkt->GetType()) {
    case Ethernet::ARP: {
      auto handled = ARP::HandleRequest(pkt, "10.0.0.4", "00:4f:33:03:ee:67");
      rsp = std::get<0>(handled);
      reqSize = std::get<1>(handled);
      err = std::get<2>(handled);
      if (err != ok) {
        fatal("Error while handling ARP packet", err);
      }
      break;
    }
    default:
      log("[INFO] Unrecognized packet type");
      continue;
    }

    err = Ethernet::Validate(pkt, reqSize);
    if (err != ok) {
      log("[INFO] Invalid packet");
      continue;
    }

    err = dev.WritePacket(pkt, rsp);
    if (err != ok) {
      fatal("Error while writing packet", err);
    }
  }

  return 0;
}
