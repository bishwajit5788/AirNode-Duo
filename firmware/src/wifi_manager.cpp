#include "wifi_manager.h"
#include <WiFi.h>
#include <DNSServer.h>

namespace WifiManager {

static DNSServer s_dns;
static const IPAddress AP_IP(AIRNODE_IP_1, AIRNODE_IP_2, AIRNODE_IP_3, AIRNODE_IP_4);
static const IPAddress AP_GW(AIRNODE_IP_1, AIRNODE_IP_2, AIRNODE_IP_3, AIRNODE_IP_4);
static const IPAddress AP_MASK(255, 255, 255, 0);

void begin() {
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(AP_IP, AP_GW, AP_MASK);
  // WPA2 (Arduino ESP32 softAP uses WPA2-PSK by default when password is set)
  WiFi.softAP(AIRNODE_AP_SSID, AIRNODE_AP_PASSWORD, AIRNODE_AP_CHANNEL, false, AIRNODE_AP_MAX_CLIENTS);

  s_dns.start(53, "*", AP_IP);

  Serial.printf("AP: %s  IP: %s\n", AIRNODE_AP_SSID, WiFi.softAPIP().toString().c_str());
}

void process() {
  s_dns.processNextRequest();
}

IPAddress apIp() {
  return WiFi.softAPIP();
}

uint8_t clientCount() {
  return WiFi.softAPgetStationNum();
}

}  // namespace WifiManager
