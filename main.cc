#include "common.h"
#include "ethernet.h"
#include "tapdevice.h"

using namespace std;

int main(int argc, char **argv) {
  auto &[dev, err] = TapDevice::New();
  if (err != 0) {
    fatal("Failed to create tap device", err);
  }

  err = dev.SetUp();
  if (err != ok) {
    fatal("Failed to bring device up", err);
  }

  err = dev.SetRoute("10.0.0.1");
  if (err != ok) {
    fatal("Failed to assign route", err);
  }

  Ethernet::Packet pkt;
  while (err = dev.NextPacket(pkt); err == ok) {
    switch (pkt.GetType()) {
    case Ethernet::ARP:
      cout << "Got ARP packet" << endl;
      break;
    case ETHERNET::IP:
      cout << "Got IP packet" << endl;
      break;
    default:
      cout << "Got unknown packet" << endl;
      break;
    }
  }

  delete dev;
}
