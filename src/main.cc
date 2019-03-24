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

  Ethernet::Packet pkt;
  while (true) {
    err = dev.ReadPacket(pkt);
    if (err != ok) {
      fatal("Failed while reading from fd", err);
    }

    char *payload = pkg.GetPayload();
    char *response = Ethernet::MTU;
    switch (pkt.GetType()) {
    case Ethernet::ARP: {
      ARP::Response r = ARP::HandleRequest(pkt.GetPayload());
      break;
    }
    case Ethernet::IP:
      log("Got IP packet");
      break;
    default:
      log("Got other packet");
      break;
    }
  }

  return 0;
}
