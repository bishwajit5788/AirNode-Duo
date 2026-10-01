#pragma once

// AirNode Duo ESP32-C3 firmware configuration.
// Verify every GPIO against your exact ESP32-C3 board before connecting an ESC,
// pump driver, or TEC driver. Do not connect a load directly to an ESP32 GPIO.

// ---------------------------------------------------------------------------
// Wi-Fi SoftAP
// ---------------------------------------------------------------------------
#define AIRNODE_AP_SSID       "AirNode-Duo"
#define AIRNODE_AP_PASSWORD   "airnode123"
#define AIRNODE_AP_CHANNEL    6
#define AIRNODE_AP_MAX_CLIENTS 4

#define AIRNODE_IP_1 192
#define AIRNODE_IP_2 168
#define AIRNODE_IP_3 4
#define AIRNODE_IP_4 1

// ---------------------------------------------------------------------------
// GPIO map (centralized — do not hard-code pins elsewhere)
// ---------------------------------------------------------------------------
// ESC signal outputs. Logic-level PWM only (50 Hz servo-style).
// ESP32-C3 LEDC supports up to 14-bit resolution in Arduino-ESP32 3.x.
#define ESC1_PIN 4
#define ESC2_PIN 5

// Driver-control outputs. Must drive MOSFET/driver stages, never the load.
// Set to -1 when the hardware is not fitted.
#define PUMP_PIN 6
#define TEC_PIN  7

// ---------------------------------------------------------------------------
// ESC PWM
// ---------------------------------------------------------------------------
#define ESC_PWM_HZ          50
#define ESC_PWM_RESOLUTION  16
#define ESC_MIN_US          1000
#define ESC_MAX_US          2000

// ---------------------------------------------------------------------------
// Timing / safety
// ---------------------------------------------------------------------------
#define ESC_ARM_TIME_MS         3000   // hold min pulse before accepting START
#define CONTROL_TIMEOUT_MS      1500   // failsafe if no heartbeat
#define HEARTBEAT_EXPECTED_MS    500   // browser target interval
#define START_RAMP_PCT_PER_S      35   // controlled ramp rate
#define COOLANT_PRIME_MS        2000   // pump run time before TEC enable
#define COOLING_MIN_MOTOR1_PCT    20   // min Motor 1 applied % for TEC
