#include "motor_control.h"

namespace MotorControl {

// Stable logical channel IDs are retained for Arduino-ESP32 2.x compatibility.
// Arduino-ESP32 3.x assigns the physical LEDC channel automatically.
static const uint8_t CH_ESC1 = 0;
static const uint8_t CH_ESC2 = 1;

static uint8_t s_m1Target = 0;
static uint8_t s_m2Target = 0;
static uint8_t s_m1Applied = 0;
static uint8_t s_m2Applied = 0;
static bool s_armed = false;
static bool s_running = false;
static bool s_failsafe = false;
static bool s_pwmReady = false;
static uint32_t s_bootMs = 0;
static uint32_t s_lastControlMs = 0;
static uint32_t s_lastRampMs = 0;

static uint32_t pulseToDuty(uint16_t us) {
  // 50 Hz => 20,000 us period. Use the configured LEDC resolution.
  const uint32_t dutyMax = (1UL << ESC_PWM_RESOLUTION) - 1UL;
  return ((uint32_t)us * dutyMax) / 20000UL;
}

static void writeEscPulse(uint8_t pin, uint8_t channel, uint16_t us) {
  us = constrain(us, (uint16_t)ESC_MIN_US, (uint16_t)ESC_MAX_US);
  const uint32_t duty = pulseToDuty(us);

#if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 3)
  // Arduino-ESP32 3.x LEDC writes are pin-based.
  (void)channel;
  ledcWrite(pin, duty);
#else
  // Arduino-ESP32 2.x LEDC writes are channel-based.
  (void)pin;
  ledcWrite(channel, duty);
#endif
}

static uint16_t percentToPulse(uint8_t pct) {
  pct = constrain(pct, (uint8_t)0, (uint8_t)100);
  return ESC_MIN_US + ((uint32_t)pct * (ESC_MAX_US - ESC_MIN_US)) / 100UL;
}

static void applyOutputs() {
  if (!s_pwmReady) return;
  writeEscPulse(ESC1_PIN, CH_ESC1, percentToPulse(s_m1Applied));
  writeEscPulse(ESC2_PIN, CH_ESC2, percentToPulse(s_m2Applied));
}



void begin() {
  s_bootMs = millis();
  s_lastRampMs = s_bootMs;
  s_lastControlMs = s_bootMs;
  s_armed = false;
  s_running = false;
  s_failsafe = false;
  s_pwmReady = false;
  s_m1Target = s_m2Target = 0;
  s_m1Applied = s_m2Applied = 0;

#if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 3)
  const bool esc1Ok = ledcAttach(ESC1_PIN, ESC_PWM_HZ, ESC_PWM_RESOLUTION);
  const bool esc2Ok = ledcAttach(ESC2_PIN, ESC_PWM_HZ, ESC_PWM_RESOLUTION);
  s_pwmReady = esc1Ok && esc2Ok;
#else
  ledcSetup(CH_ESC1, ESC_PWM_HZ, ESC_PWM_RESOLUTION);
  ledcSetup(CH_ESC2, ESC_PWM_HZ, ESC_PWM_RESOLUTION);
  ledcAttachPin(ESC1_PIN, CH_ESC1);
  ledcAttachPin(ESC2_PIN, CH_ESC2);
  s_pwmReady = true;
#endif

  if (!s_pwmReady) {
    Serial.println("ERROR: ESC PWM initialization failed; motors locked off.");
    return;
  }

  writeEscPulse(ESC1_PIN, CH_ESC1, ESC_MIN_US);
  writeEscPulse(ESC2_PIN, CH_ESC2, ESC_MIN_US);
  Serial.printf("ESC PWM ready: %u Hz, %u-bit, outputs held at %u us.\n",
                ESC_PWM_HZ, ESC_PWM_RESOLUTION, ESC_MIN_US);
}

void update() {
  if (!s_pwmReady) return;

  if (!s_armed && (millis() - s_bootMs) >= ESC_ARM_TIME_MS) {
    s_armed = true;
    Serial.println("ESC arming complete.");
  }

  if (s_running && !hasRecentControl()) {
    Serial.println("FAILSAFE: control heartbeat timeout");
    triggerFailsafe();
  }

  if (!s_armed || !s_running) {
    if (s_m1Applied || s_m2Applied) {
      s_m1Applied = s_m2Applied = 0;
      applyOutputs();
    }
    return;
  }

  const uint32_t now = millis();
  const uint32_t dt = now - s_lastRampMs;
  if (!dt) return;
  s_lastRampMs = now;

  const uint8_t step = max<uint8_t>(1, (uint8_t)((START_RAMP_PCT_PER_S * dt) / 1000UL));

  if (s_m1Applied < s_m1Target)
    s_m1Applied = min<uint8_t>(s_m1Target, (uint8_t)(s_m1Applied + step));
  else if (s_m1Applied > s_m1Target)
    s_m1Applied = (s_m1Applied > step) ? (uint8_t)(s_m1Applied - step) : 0;

  if (s_m2Applied < s_m2Target)
    s_m2Applied = min<uint8_t>(s_m2Target, (uint8_t)(s_m2Applied + step));
  else if (s_m2Applied > s_m2Target)
    s_m2Applied = (s_m2Applied > step) ? (uint8_t)(s_m2Applied - step) : 0;

  applyOutputs();
}

bool isArmed() { return s_armed && s_pwmReady; }
bool isRunning() { return s_running; }
bool isFailsafe() { return s_failsafe; }

uint8_t motor1Target() { return s_m1Target; }
uint8_t motor2Target() { return s_m2Target; }
uint8_t motor1Applied() { return s_m1Applied; }
uint8_t motor2Applied() { return s_m2Applied; }

void setTargets(uint8_t m1, uint8_t m2) {
  s_m1Target = constrain(m1, (uint8_t)0, (uint8_t)100);
  s_m2Target = constrain(m2, (uint8_t)0, (uint8_t)100);
  touchControl();
}

void start() {
  if (!isArmed()) return;
  s_failsafe = false;
  s_running = true;
  s_lastRampMs = millis();
  touchControl();
}

void stop() {
  s_running = false;
  s_m1Target = s_m2Target = 0;
  s_m1Applied = s_m2Applied = 0;
  applyOutputs();
  touchControl();
}

void triggerFailsafe() {
  s_failsafe = true;
  s_running = false;
  s_m1Target = s_m2Target = 0;
  s_m1Applied = s_m2Applied = 0;
  applyOutputs();
}

void touchControl() {
  s_lastControlMs = millis();
}

bool hasRecentControl() {
  return (millis() - s_lastControlMs) <= CONTROL_TIMEOUT_MS;
}

}  // namespace MotorControl
