#include "wifi_manager.h"
#include <WiFi.h>
#include <DNSServer.h>

namespace WifiManager {

static DNSServer s_dns;
static const IPAddress AP_IP(AIRNODE_IP_1, AIRNODE_IP_2, AIRNODE_IP_3, AIRNODE_IP_4);
static const IPAddress AP_GW(AIRNODE_IP_1, AIRNODE_IP_2, AIRNODE_IP_3, AIRNODE_IP_4);
static const IPAddress AP_MASK(255, 255, 255, 0);

void begin() {
  // SoftAP only — no STA, no internet dependency.
  WiFi.persistent(false);
  WiFi.disconnect(true, true);
  delay(50);
  WiFi.mode(WIFI_AP);
  delay(50);

  // Fixed AP address so captive redirects always target 192.168.4.1
  if (!WiFi.softAPConfig(AP_IP, AP_GW, AP_MASK)) {
    Serial.println("WARNING: softAPConfig failed");
  }

  // WPA2-PSK when password is set (Arduino-ESP32 SoftAP default).
  const bool apOk = WiFi.softAP(
      AIRNODE_AP_SSID,
      AIRNODE_AP_PASSWORD,
      AIRNODE_AP_CHANNEL,
      false,  // not hidden
      AIRNODE_AP_MAX_CLIENTS);

  if (!apOk) {
    Serial.println("ERROR: SoftAP start failed");
  }

  delay(100);

  // Wildcard DNS: every hostname → AP IP (drives captive-portal detection).
  s_dns.start(53, "*", AP_IP);

  Serial.printf("AP: %s  IP: %s  DNS captive: on\n",
                AIRNODE_AP_SSID,
                WiFi.softAPIP().toString().c_str());
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
