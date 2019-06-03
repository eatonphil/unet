#include "common.h"
#include "ethernet.h"
#include "ipv4.h"
#include "tapdevice.h"
#include "tcp.h"

using namespace std;

vector<uint8_t> webServer() {
  const string response = "HTTP/1.1 200 OK\r\n"
                          "Content-Type: text/html\r\n\r\n"
                          "<h1>Hello beautiful world!</h1>";
  return vector<uint8_t>(response.begin(), response.end());
}

int main(int argc, char **argv) {
  HTTP::Handler httpHandler();
  httpHandler.AddHandler(webServer);

  TCP::Handler tcpHandler();
  tcpHandler.AddHandler(8080, httpServer);

  IPv4::Handler ipv4Handler(tcpHandler);

  const uint8_t mac[6] = {0x00, 0x4f, 0x33, 0x03, 0xee, 0x67};
  const uint8_t ipv4[4] = {10, 0, 0, 4};
  TapDevice::TapDevice dev(mac, addr, ipv4Handler);

  error err = dev.Init();
  if (err != ok) {
    fatal("Failed to create, bring up tap device", err);
  }

  log("[INFO] Created tap device");

  dev.HandleRequests();

  return 0;
}
