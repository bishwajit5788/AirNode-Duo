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

static void noCache() {
  server.sendHeader("Cache-Control", "no-store, no-cache, must-revalidate, max-age=0");
  server.sendHeader("Pragma", "no-cache");
}

static String statusJson() {
  JsonDocument doc;
  doc["armed"] = MotorControl::isArmed();
  doc["running"] = MotorControl::isRunning();
  doc["failsafe"] = MotorControl::isFailsafe();
  doc["motor1_target"] = MotorControl::motor1Target();
  doc["motor1_applied"] = MotorControl::motor1Applied();
  doc["motor2_target"] = MotorControl::motor2Target();
  doc["motor2_applied"] = MotorControl::motor2Applied();
  doc["cooling_target"] = CoolingControl::level();
  doc["cooling_requested"] = CoolingControl::isRequested();
  doc["pump"] = CoolingControl::isPumpOn();
  doc["tec"] = CoolingControl::isTecOn();
  doc["clients"] = WifiManager::clientCount();
  doc["wifi_clients"] = WifiManager::clientCount();
  doc["ip"] = WifiManager::apIp().toString();
  String out;
  serializeJson(doc, out);
  return out;
}

static void sendStatus() {
  noCache();
  server.send(200, "application/json", statusJson());
}

static void handleRoot() {
  noCache();
  server.send_P(200, "text/html", INDEX_HTML);
}

// Serve portal page so phone captive UI has something to display.
static void handlePortalPage() {
  noCache();
  server.send_P(200, "text/html", INDEX_HTML);
}

static void handleCaptiveRedirect() {
  noCache();
  server.sendHeader("Location", String("http://") + WifiManager::apIp().toString() + "/", true);
  server.send(302, "text/plain", "Redirecting to AirNode Duo");
}

// Android connectivity check: redirect into portal (opens captive sheet on many devices).
static void handleGenerate204() {
  handleCaptiveRedirect();
}

// Apple captive network: serve portal HTML (detection fails closed → login sheet).
static void handleHotspotDetect() {
  handlePortalPage();
}

static void handleConnectTest() {
  handleCaptiveRedirect();
}

static void handleNcsi() {
  handleCaptiveRedirect();
}

static void handleCanonical() {
  handlePortalPage();
}

static void handleSuccess() {
  handlePortalPage();
}

static bool parseUint0_100(const String& s, uint8_t& out) {
  if (s.length() == 0) return false;
  for (size_t i = 0; i < s.length(); i++) {
    if (s[i] < '0' || s[i] > '9') return false;
  }
  long v = s.toInt();
  if (v < 0 || v > 100) return false;
  out = (uint8_t)v;
  return true;
}

// Prefer form args; fall back to JSON body (application/json or plain).
static bool loadJsonBody(JsonDocument& doc) {
  if (!server.hasArg("plain")) return false;
  const String& body = server.arg("plain");
  if (body.length() == 0) return false;
  DeserializationError err = deserializeJson(doc, body);
  return !err;
}

static void handleControl() {
  uint8_t m1 = 0, m2 = 0;
  bool got = false;

  if (server.hasArg("m1") && server.hasArg("m2")) {
    if (!parseUint0_100(server.arg("m1"), m1) || !parseUint0_100(server.arg("m2"), m2)) {
      server.send(400, "application/json", "{\"error\":\"m1 and m2 must be 0..100\"}");
      return;
    }
    got = true;
  } else {
    JsonDocument doc;
    if (loadJsonBody(doc)) {
      if (!doc["motor1"].is<int>() && !doc["m1"].is<int>()) {
        server.send(400, "application/json", "{\"error\":\"motor1/m1 required\"}");
        return;
      }
      int v1 = doc["motor1"].is<int>() ? doc["motor1"].as<int>() : doc["m1"].as<int>();
      int v2 = doc["motor2"].is<int>() ? doc["motor2"].as<int>()
               : (doc["m2"].is<int>() ? doc["m2"].as<int>() : -1);
      if (v2 < 0) {
        server.send(400, "application/json", "{\"error\":\"motor2/m2 required\"}");
        return;
      }
      if (v1 < 0 || v1 > 100 || v2 < 0 || v2 > 100) {
        server.send(400, "application/json", "{\"error\":\"motor values must be 0..100\"}");
        return;
      }
      m1 = (uint8_t)v1;
      m2 = (uint8_t)v2;
      got = true;
    }
  }

  if (!got) {
    server.send(400, "application/json", "{\"error\":\"m1/m2 form or JSON motor1/motor2 required\"}");
    return;
  }

  MotorControl::touchControl();

  if (!MotorControl::isRunning() && (m1 || m2)) {
    server.send(409, "application/json", "{\"error\":\"start required\"}");
    return;
  }

  MotorControl::setTargets(m1, m2);
  noCache();
  server.send(200, "application/json", statusJson());
}

static void handleMaster() {
  uint8_t sp = 0;
  bool got = false;

  if (server.hasArg("speed")) {
    if (!parseUint0_100(server.arg("speed"), sp)) {
      server.send(400, "application/json", "{\"error\":\"speed must be 0..100\"}");
      return;
    }
    got = true;
  } else {
    JsonDocument doc;
    if (loadJsonBody(doc)) {
      int v = doc["master"].is<int>() ? doc["master"].as<int>()
              : (doc["speed"].is<int>() ? doc["speed"].as<int>() : -1);
      if (v < 0 || v > 100) {
        server.send(400, "application/json", "{\"error\":\"master/speed must be 0..100\"}");
        return;
      }
      sp = (uint8_t)v;
      got = true;
    }
  }

  if (!got) {
    server.send(400, "application/json", "{\"error\":\"speed= or JSON master required\"}");
    return;
  }

  MotorControl::touchControl();
  if (!MotorControl::isRunning() && sp) {
    server.send(409, "application/json", "{\"error\":\"start required\"}");
    return;
  }
  MotorControl::setTargets(sp, sp);
  noCache();
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
  noCache();
  server.send(200, "application/json", statusJson());
}

static void handleStop() {
  MotorControl::stop();
  CoolingControl::forceOff();
  noCache();
  server.send(200, "application/json", statusJson());
}

static void handleHeartbeat() {
  MotorControl::touchControl();
  noCache();
  server.send(200, "application/json", statusJson());
}

static void handleCooling() {
  uint8_t level = 0;
  bool got = false;

  if (server.hasArg("cooling")) {
    if (!parseUint0_100(server.arg("cooling"), level)) {
      server.send(400, "application/json", "{\"error\":\"cooling must be 0..100\"}");
      return;
    }
    got = true;
  } else if (server.hasArg("on")) {
    String v = server.arg("on");
    if (v != "0" && v != "1") {
      server.send(400, "application/json", "{\"error\":\"on=0 or on=1\"}");
      return;
    }
    level = (v == "1") ? 100 : 0;
    got = true;
  } else {
    JsonDocument doc;
    if (loadJsonBody(doc)) {
      if (doc["cooling"].is<int>()) {
        int v = doc["cooling"].as<int>();
        if (v < 0 || v > 100) {
          server.send(400, "application/json", "{\"error\":\"cooling must be 0..100\"}");
          return;
        }
        level = (uint8_t)v;
        got = true;
      } else if (doc["on"].is<int>() || doc["on"].is<bool>()) {
        level = doc["on"].as<bool>() || doc["on"].as<int>() ? 100 : 0;
        got = true;
      }
    }
  }

  if (!got) {
    server.send(400, "application/json", "{\"error\":\"cooling=0..100 or on=0|1 required\"}");
    return;
  }

  MotorControl::touchControl();
  CoolingControl::setLevel(level);
  noCache();
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

  // Captive-portal detection — redirect or serve portal so OS shows sheet
  server.on("/generate_204", HTTP_GET, handleGenerate204);
  server.on("/gen_204", HTTP_GET, handleGenerate204);
  server.on("/hotspot-detect.html", HTTP_GET, handleHotspotDetect);
  server.on("/library/test/success.html", HTTP_GET, handleHotspotDetect);
  server.on("/connecttest.txt", HTTP_GET, handleConnectTest);
  server.on("/ncsi.txt", HTTP_GET, handleNcsi);
  server.on("/canonical.html", HTTP_GET, handleCanonical);
  server.on("/success.txt", HTTP_GET, handleSuccess);
  server.on("/fwlink", HTTP_GET, handleCaptiveRedirect);
  server.on("/redirect", HTTP_GET, handleCaptiveRedirect);

  server.onNotFound(handleCaptiveRedirect);
  server.begin();
}

void process() {
  server.handleClient();
}

}  // namespace WebServerApp
