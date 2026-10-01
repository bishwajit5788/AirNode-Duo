#include "cooling_control.h"

namespace CoolingControl {

static bool s_requested = false;
static bool s_pumpOn = false;
static bool s_tecOn = false;
static uint32_t s_primeStartMs = 0;

void begin() {
  s_requested = false;
  s_pumpOn = false;
  s_tecOn = false;
  s_primeStartMs = 0;

#if PUMP_PIN >= 0
  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW);
#endif
#if TEC_PIN >= 0
  pinMode(TEC_PIN, OUTPUT);
  digitalWrite(TEC_PIN, LOW);
#endif
}

static void setPump(bool on) {
#if PUMP_PIN >= 0
  digitalWrite(PUMP_PIN, on ? HIGH : LOW);
#endif
  s_pumpOn = on;
}

static void setTec(bool on) {
#if TEC_PIN >= 0
  digitalWrite(TEC_PIN, on ? HIGH : LOW);
#endif
  s_tecOn = on;
}

void forceOff() {
  s_requested = false;
  setTec(false);
  setPump(false);
  s_primeStartMs = 0;
}

void setRequested(bool on) {
  s_requested = on;
  if (!on) {
    setTec(false);
    setPump(false);
    s_primeStartMs = 0;
  }
}

void update(bool systemRunning, uint8_t motor1AppliedPct) {
  // Safety: no cooling unless system is running and Motor 1 has enough airflow
  if (!s_requested || !systemRunning || motor1AppliedPct < COOLING_MIN_MOTOR1_PCT) {
    if (s_tecOn) setTec(false);
    if (s_pumpOn) setPump(false);
    s_primeStartMs = 0;
    return;
  }

  // Start pump first
  if (!s_pumpOn) {
    setPump(true);
    s_primeStartMs = millis();
    return;
  }

  // After prime delay, enable TEC
  if (!s_tecOn && (millis() - s_primeStartMs) >= COOLANT_PRIME_MS) {
    setTec(true);
  }
}

bool isRequested() { return s_requested; }
bool isPumpOn() { return s_pumpOn; }
bool isTecOn() { return s_tecOn; }
bool isPriming() { return s_pumpOn && !s_tecOn && s_requested; }

}  // namespace CoolingControl
