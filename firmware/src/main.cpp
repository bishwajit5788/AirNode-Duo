#include <Arduino.h>
#include "config.h"
#include "motor_control.h"
#include "cooling_control.h"
#include "wifi_manager.h"
#include "web_server.h"

void setup() {
  Serial.begin(115200);
  delay(50);

  Serial.println();
  Serial.println("AirNode Duo firmware booting...");

  MotorControl::begin();
  CoolingControl::begin();
  WifiManager::begin();
  WebServerApp::begin();

  Serial.println("Ready. Open http://192.168.4.1 after joining AirNode-Duo.");
}

void loop() {
  WifiManager::process();
  WebServerApp::process();

  MotorControl::update();

  // Cooling supervision uses live motor state
  CoolingControl::update(MotorControl::isRunning(), MotorControl::motor1Applied());

  // If failsafe just triggered motors, ensure cooling is off
  if (MotorControl::isFailsafe() && CoolingControl::isRequested()) {
    CoolingControl::forceOff();
  }

  delay(2);
}
