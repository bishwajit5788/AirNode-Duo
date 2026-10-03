# AirNode Duo — Firmware

PlatformIO firmware for the ESP32-C3 SoftAP controller.

For full project architecture, safety, and hardware context, see the **[root README](../README.md)**.

---

## Quick start

```bash
cd firmware
pio run                 # build
pio run -t upload       # flash
pio device monitor      # 115200 baud
```

---

## Wi-Fi

| Setting | Value |
|---------|--------|
| SSID | `AirNode-Duo` |
| Password | `airnode123` |
| Control URL | **http://192.168.4.1** |

No internet required. Captive-portal DNS/redirects are implemented; phone OS behavior varies — use the URL if the page does not open automatically.

---

## GPIO defaults (`src/config.h`)

| Function | GPIO |
|----------|-----:|
| ESC #1 | 4 |
| ESC #2 | 5 |
| Pump driver | 6 |
| TEC driver | 7 |

Verify against your board. Drivers only — never load GPIO directly with pump/Peltier/motor power.

---

## PWM compatibility

The ESC outputs use **50 Hz servo-style PWM** with **14-bit LEDC resolution** on ESP32-C3.

The implementation supports the current Arduino-ESP32 3.x LEDC API (`ledcAttach` / pin-based `ledcWrite`) and retains a compatibility path for Arduino-ESP32 2.x.

If LEDC initialization fails, the firmware locks the motor outputs off and does not arm the controller.

## API

| Method | Path | Body / notes |
|--------|------|----------------|
| GET | `/` | Control UI |
| GET | `/api/status` | JSON |
| POST | `/api/start` | After 3 s arm |
| POST | `/api/stop` | Motors + cooling off |
| POST | `/api/control` | `m1=0..100&m2=0..100` |
| POST | `/api/master` | `speed=0..100` |
| POST | `/api/heartbeat` | ~500 ms from browser |
| POST | `/api/cooling` | `cooling=0..100` or `on=0|1` (also JSON) |

Failsafe: no valid control for **1.5 s** while running → stop motors and cooling; no auto-restart.

---

## Source layout

```text
src/
  main.cpp
  config.h
  wifi_manager.*
  web_server.*
  motor_control.*
  cooling_control.*
  web_ui.*
```

---

## Bench test (propellers **removed**)

1. Power ESP32 from 12 V → 5 V buck; ESCs from 12 V per manufacturer wiring.  
2. Flash, join **AirNode-Duo**, open **http://192.168.4.1**.  
3. Wait until UI shows **ARMED**.  
4. START at 0%; raise Motor 1 then Motor 2 slowly.  
5. Drop Wi-Fi / close page → failsafe within ~1.5 s.  
6. Reconnect → motors stay off until **START**.  
7. Cooling: Motor 1 ≥ 20% → COOLING ON → pump then TEC after ~2 s.  

Software failsafe is not a substitute for a physical emergency disconnect.

**Build status:** use the GitHub Actions workflow as the authoritative compile check; this document does not claim a successful build until CI reports one.

Only after this passes should propellers and loaded tests begin (see root README).
