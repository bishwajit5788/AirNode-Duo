#include "cooling_control.h"

namespace CoolingControl {

static uint8_t s_level = 0;
static bool s_pumpOn = false;
static bool s_tecOn = false;
static uint32_t s_primeStartMs = 0;

void begin() {
  s_level = 0;
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
  s_level = 0;
  setTec(false);
  setPump(false);
  s_primeStartMs = 0;
}

void setLevel(uint8_t pct) {
  s_level = constrain(pct, (uint8_t)0, (uint8_t)100);
  if (s_level == 0) {
    setTec(false);
    setPump(false);
    s_primeStartMs = 0;
  }
}

void setRequested(bool on) {
  setLevel(on ? 100 : 0);
}

void update(bool systemRunning, uint8_t motor1AppliedPct) {
  // Safety: no cooling unless requested, system running, and Motor 1 airflow OK.
  // No flow/temperature sensors on this prototype — interlocks are software only.
  if (s_level == 0 || !systemRunning || motor1AppliedPct < COOLING_MIN_MOTOR1_PCT) {
    if (s_tecOn) setTec(false);
    if (s_pumpOn) setPump(false);
    s_primeStartMs = 0;
    return;
  }

  if (!s_pumpOn) {
    setPump(true);
    s_primeStartMs = millis();
    return;
  }

  // TEC is binary on this hardware (MOSFET driver). Level > 0 only gates enable.
  if (!s_tecOn && (millis() - s_primeStartMs) >= COOLANT_PRIME_MS) {
    setTec(true);
  }
}

uint8_t level() { return s_level; }
bool isRequested() { return s_level > 0; }
bool isPumpOn() { return s_pumpOn; }
bool isTecOn() { return s_tecOn; }
bool isPriming() { return s_pumpOn && !s_tecOn && s_level > 0; }

}  // namespace CoolingControl
