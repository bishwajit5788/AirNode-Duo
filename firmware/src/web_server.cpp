#include "web_server.h"
#include "web_ui.h"
#include "motor_control.h"
#include "cooling_control.h"
#include "wifi_manager.h"
#include "config.h"

#include <WebServer.h>
#include <ArduinoJson.h>

namespace WebServerApp {

static WebServer server(80);

static String statusJson() {
  JsonDocument doc;
  doc["armed"] = MotorControl::isArmed();
  doc["running"] = MotorControl::isRunning();
  doc["failsafe"] = MotorControl::isFailsafe();
  doc["motor1_target"] = MotorControl::motor1Target();
  doc["motor1_applied"] = MotorControl::motor1Applied();
  doc["motor2_target"] = MotorControl::motor2Target();
  doc["motor2_applied"] = MotorControl::motor2Applied();
  doc["cooling_requested"] = CoolingControl::isRequested();
  doc["pump"] = CoolingControl::isPumpOn();
  doc["tec"] = CoolingControl::isTecOn();
  doc["clients"] = WifiManager::clientCount();
  doc["ip"] = WifiManager::apIp().toString();
  String out;
  serializeJson(doc, out);
  return out;
}

static void sendStatus() {
  server.send(200, "application/json", statusJson());
}

static void handleRoot() {
  server.send_P(200, "text/html", INDEX_HTML);
}

static void handleCaptiveRedirect() {
  server.sendHeader("Location", String("http://") + WifiManager::apIp().toString() + "/", true);
  server.send(302, "text/plain", "AirNode Duo");
}

// Android: expect 204; redirect still works for captive portal popup
static void handleGenerate204() {
  handleCaptiveRedirect();
}

static void handleHotspotDetect() {
  // Apple: serve success-like content or redirect
  handleCaptiveRedirect();
}

static void handleConnectTest() {
  handleCaptiveRedirect();
}

static void handleNcsi() {
  // Windows NCSI
  handleCaptiveRedirect();
}

static void handleCanonical() {
  handleCaptiveRedirect();
}

static void handleSuccess() {
  handleCaptiveRedirect();
}

static bool parseUint0_100(const String& s, uint8_t& out) {
  if (s.length() == 0) return false;
  // Reject non-numeric / negative
  for (size_t i = 0; i < s.length(); i++) {
    if (s[i] < '0' || s[i] > '9') return false;
  }
  long v = s.toInt();
  if (v < 0 || v > 100) return false;
  out = (uint8_t)v;
  return true;
}

static void handleControl() {
  if (!server.hasArg("m1") || !server.hasArg("m2")) {
    server.send(400, "application/json", "{\"error\":\"m1 and m2 required (0..100)\"}");
    return;
  }
  uint8_t m1, m2;
  if (!parseUint0_100(server.arg("m1"), m1) || !parseUint0_100(server.arg("m2"), m2)) {
    server.send(400, "application/json", "{\"error\":\"m1 and m2 must be 0..100\"}");
    return;
  }

  MotorControl::touchControl();

  if (!MotorControl::isRunning() && (m1 || m2)) {
    server.send(409, "application/json", "{\"error\":\"start required\"}");
    return;
  }

  MotorControl::setTargets(m1, m2);
  server.send(200, "application/json", statusJson());
}

static void handleMaster() {
  if (!server.hasArg("speed")) {
    server.send(400, "application/json", "{\"error\":\"speed=0..100 required\"}");
    return;
  }
  uint8_t sp;
  if (!parseUint0_100(server.arg("speed"), sp)) {
    server.send(400, "application/json", "{\"error\":\"speed must be 0..100\"}");
    return;
  }

  MotorControl::touchControl();
  if (!MotorControl::isRunning() && sp) {
    server.send(409, "application/json", "{\"error\":\"start required\"}");
    return;
  }
  MotorControl::setTargets(sp, sp);
  server.send(200, "application/json", statusJson());
}

static void handleStart() {
  if (!MotorControl::isArmed()) {
    server.send(503, "application/json", "{\"error\":\"ESC arming period not complete\"}");
    return;
  }
  if (WifiManager::clientCount() == 0) {
    server.send(403, "application/json", "{\"error\":\"no control client connected\"}");
    return;
  }
  MotorControl::start();
  server.send(200, "application/json", statusJson());
}

static void handleStop() {
  MotorControl::stop();
  CoolingControl::forceOff();
  server.send(200, "application/json", statusJson());
}

static void handleHeartbeat() {
  MotorControl::touchControl();
  server.send(200, "application/json", statusJson());
}

static void handleCooling() {
  if (!server.hasArg("on")) {
    server.send(400, "application/json", "{\"error\":\"on=0 or on=1 required\"}");
    return;
  }
  String v = server.arg("on");
  if (v != "0" && v != "1") {
    server.send(400, "application/json", "{\"error\":\"on=0 or on=1 required\"}");
    return;
  }
  MotorControl::touchControl();
  CoolingControl::setRequested(v == "1");
  server.send(200, "application/json", statusJson());
}

void begin() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/status", HTTP_GET, sendStatus);
  server.on("/api/control", HTTP_POST, handleControl);
  server.on("/api/master", HTTP_POST, handleMaster);
  server.on("/api/start", HTTP_POST, handleStart);
  server.on("/api/stop", HTTP_POST, handleStop);
  server.on("/api/heartbeat", HTTP_POST, handleHeartbeat);
  server.on("/api/cooling", HTTP_POST, handleCooling);

  // Captive portal detection endpoints
  server.on("/generate_204", HTTP_GET, handleGenerate204);
  server.on("/gen_204", HTTP_GET, handleGenerate204);
  server.on("/hotspot-detect.html", HTTP_GET, handleHotspotDetect);
  server.on("/library/test/success.html", HTTP_GET, handleHotspotDetect);
  server.on("/connecttest.txt", HTTP_GET, handleConnectTest);
  server.on("/ncsi.txt", HTTP_GET, handleNcsi);
  server.on("/canonical.html", HTTP_GET, handleCanonical);
  server.on("/success.txt", HTTP_GET, handleSuccess);
  server.on("/fwlink", HTTP_GET, handleCaptiveRedirect);

  server.onNotFound(handleCaptiveRedirect);
  server.begin();
}

void process() {
  server.handleClient();
}

}  // namespace WebServerApp
