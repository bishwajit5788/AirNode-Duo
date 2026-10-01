# AirNode Duo

> **ESP32-C3 controlled dual-BLDC personal airflow and thermoelectric cooling system**

**Motto:** *Cool the person, not the room — compact airflow where it matters.*

Repository: [github.com/bishwajit5788/AirNode-Duo](https://github.com/bishwajit5788/AirNode-Duo)

---

## 1. Purpose

AirNode Duo is a compact personal airflow system meant to mount above a person inside:

- a mosquito net  
- a single-bed sleeping area  
- a lightweight camping tent  
- a compact temporary sleeping space  

Two BLDC motors with propellers produce **downward airflow** similar to a ceiling fan. The goal is **perceived cooling through forced convection** — moving useful air toward the person — not refrigerating a room.

**This system is not:**

- a room air conditioner  
- a refrigerator for the sleeping space  
- a device that produces ice-cold air  

Motor 1 may use an experimental thermoelectric liquid loop to slightly cool the air it moves. Motor 2 remains a normal, unobstructed airflow motor.

---

## 2. Physical architecture

```text
                  MOSQUITO NET / TENT ROOF
        ===========================================

              MOTOR 1             MOTOR 2
           2312 920KV CW       2312 920KV CW
                │                   │
              PROP 1              PROP 2
                ↓                   ↓
             cooled              normal
             airflow             airflow
                ↓                   ↓
              ↓↓↓↓↓             ↓↓↓↓↓
                 \               /
                  \             /
                   \           /
                    PERSON
```

- Two motors on a **lightweight, symmetric, roof-mounted frame**
- Heavy AC–DC PSU stays **outside** the hanging assembly
- Only low-voltage DC wiring runs up to the roof unit

---

## 3. Motors

| Item | Value |
|------|--------|
| Part | Readytosky **2312 920KV CW** BLDC |
| Quantity | 2 |
| KV | ~920 RPM/V |
| Mass | ~48 g each |
| Diameter / body | ~27.7 mm OD × ~26 mm |
| No-load current | ~0.45 A at 10 V |
| Max continuous current | ~14 A (nameplate) |
| Max continuous power | ~220 W (nameplate) |
| Voltage class | ~2S–4S equivalent |
| Suggested prop families | 9443, **9450** (start), 1045 |

**Do not assume** “30 A ESC ⇒ 30 A motor current.” Actual current depends on propeller, voltage, throttle, blockage, and mounting. **Measure** current on the finished hardware.

---

## 4. Propellers

| Candidate | Role |
|-----------|------|
| **9450** | Primary starting candidate |
| 9443 | Lower-load baseline |
| 1045 | Larger swept-area comparison |

Larger props can raise current, temperature, vibration, noise, and stress. Final choice requires measured current, temperatures, vibration, airflow, and acoustics — not automatic selection of the largest prop.

---

## 5. ESCs

| Item | Value |
|------|--------|
| Part | Favorite **LittleBee 30A-S OPTO** × 2 |
| Class | 30 A, BLHeli_S, 2–6S |
| BEC | **None (OPTO)** |

- ESC #1 → Motor #1  
- ESC #2 → Motor #2  
- ESCs **do not** power the ESP32  
- ESP32 is powered by a separate **12 V → 5 V buck converter**

---

## 6. Main power supply

| Item | Value |
|------|--------|
| PSU | 7SEVEN/MJF (or equivalent) |
| Rating | **12 V / 50 A / 600 W**, IP44 |

- PSU stays **outside** the hanging assembly  
- AC mains path: fuse → master switch → SMPS  

**Nameplate motor budget (upper bound, not measured load):**  
2 × 14 A = 28 A → 28 A × 12 V ≈ **336 W**.  
The 600 W PSU has headroom on paper; that is **not** validation of continuous 600 W system use. Other loads: Peltier, pump, hot-side fan, ESP32, losses. **Measure** real current and thermal behavior.

---

## 7. Electrical architecture

```text
                      230 V AC MAINS
                            │
                [Fuse + Master Switch]
                            │
                ┌───────────▼───────────┐
                │ 12 V / 50 A / 600 W   │
                │      AC-DC SMPS        │
                └───────────┬───────────┘
                            │
          ┌─────────┬───────┼───────┬─────────┐
          │         │       │       │         │
       ESC #1    ESC #2  Peltier  Pump   12→5 V buck
          │         │    driver   │         │
       Motor 1   Motor 2    TEC   │      ESP32-C3
          │         │             │      Wi-Fi AP
       PROP 1    PROP 2           │         │
                                  │       PHONE
                             (+ hot-side fan)
```

ESP32 GPIOs only drive **signal / driver** circuits. They must **not** power motors, ESCs, Peltier, or pump directly.

---

## 8. ESP32-C3 controller

**Board:** ESP32-C3 Mini (or compatible).

**Responsibilities:**

- Wi-Fi SoftAP + captive-portal helpers  
- Local web UI and HTTP API  
- Motor targets, ramp, START/STOP  
- Cooling sequence and interlocks  
- Heartbeat failsafe  
- Future sensor telemetry (not invented until sensors exist)

---

## 9. Wi-Fi user experience

1. Power on → ESP32 boots  
2. SoftAP appears: **AirNode-Duo**  
3. User joins the network  
4. Captive-portal detection *may* open the control page (OS-dependent)  
5. Always available: **http://192.168.4.1**  

**No internet required.** Core control is fully local.

| Setting | Value |
|---------|--------|
| SSID | `AirNode-Duo` |
| Password | `airnode123` |
| AP IP | `192.168.4.1` |
| Subnet | `255.255.255.0` |

Captive helpers include DNS wildcard → `192.168.4.1` and common paths such as `/generate_204`, `/hotspot-detect.html`, `/connecttest.txt`, `/ncsi.txt`, `/canonical.html`, `/success.txt`. **Do not claim** every phone OS will auto-open the page.

---

## 10. Phone control interface

Mobile-first local page:

- Title: **AirNode Duo**  
- Connection: ● CONNECTED / FAILSAFE / DISCONNECTED  
- IP: 192.168.4.1  
- System: ARMING / ARMED / RUNNING / STOPPED / FAILSAFE  
- Motor 1 & Motor 2: 2312 920KV CW, 0–100% sliders, target + applied %  
- Master speed: sets both targets (then adjustable independently)  
- **START** / **STOP** (STOP always prominent)  
- Thermoelectric cooling: ON/OFF, pump and TEC status  

---

## 11. Motor control behavior

| Phase | Behavior |
|-------|----------|
| Boot | Both ESC outputs **1000 µs**; motors stay off |
| Arm | Wait **3 s** before START is accepted |
| START | Explicit user action only; controlled **ramp** toward targets |
| STOP | Motors → off; cooling forced off |
| Failsafe | No valid control/heartbeat for **1.5 s** while running → motors + cooling off |

Browser sends heartbeats ~**500 ms**. After failsafe or reconnect, motors **do not** auto-restart; user must press **START** again.

Firmware ramps targets; it does **not** jump instantly from 0% to a high throttle on START.

---

## 12. Thermoelectric cooling (Motor 1)

**Concept:** Peltier cools **coolant**, not air directly. Coolant cools a copper coil; Motor 1 airflow passes over the coil.

```text
12 V Peltier → cold-side block → coolant → pump → tubing
    → copper coil (behind Motor 1 prop) → airflow → person
```

| Item | Notes |
|------|--------|
| Peltier | ~12 V / ~2 A class (~20 W ideal); **not** room AC |
| Hot side | Aluminium heatsink + dedicated fan; heat must leave the cold path |
| Coil | Must **not** fully block the prop disc (load, current, heat rise) |
| Tube size | If “1 mm” is **internal** diameter, flow resistance may be excessive — verify ID vs wall |

### Cooling sequence (firmware)

1. COOLING ON  
2. Motor 1 running and applied ≥ ~**20%**  
3. Pump ON  
4. Wait ~**2 s** (prime)  
5. TEC driver ON → **ACTIVE**  

If Motor 1 stops / drops below threshold, or STOP/failsafe: **TEC off**, **pump off**.

### Cooling safety gaps (not yet instrumented)

Firmware does **not** yet verify coolant flow, hot-side temperature, cold-side temperature, or condensation. Future work: sensors, flow/pump-fail detection, condensation management. Avoid uncontrolled leakage above person, ESP32, ESCs, or power joints (sealed reservoir, secure fittings).

---

## 13. GPIO map (firmware defaults)

Centralized in `firmware/src/config.h` — do not scatter pin numbers.

| Function | GPIO | Notes |
|----------|-----:|--------|
| ESC #1 signal | 4 | Logic-level PWM only |
| ESC #2 signal | 5 | Logic-level PWM only |
| Pump driver | 6 | MOSFET/driver stage only |
| TEC driver | 7 | MOSFET/driver stage only |

Verify against the **exact** board pinout before connecting loads.

---

## 14. Firmware layout

```text
firmware/
├── platformio.ini
├── README.md          ← build, flash, API, bench checklist
└── src/
    ├── main.cpp
    ├── config.h
    ├── wifi_manager.*
    ├── web_server.*
    ├── motor_control.*
    ├── cooling_control.*
    └── web_ui.*
```

Responsibilities stay separated: Wi-Fi, web, motors, cooling, configuration, UI.

### HTTP API (local only)

| Method | Path | Role |
|--------|------|------|
| GET | `/` | Control page |
| GET | `/api/status` | JSON state |
| POST | `/api/start` | Start (after arm) |
| POST | `/api/stop` | Hard stop |
| POST | `/api/control` | `m1`, `m2` ∈ 0…100 |
| POST | `/api/master` | `speed` ∈ 0…100 |
| POST | `/api/heartbeat` | Keep-alive |
| POST | `/api/cooling` | `on=0` or `on=1` |

All inputs validated and clamped. No fake temperature/current fields in JSON.

Example status:

```json
{
  "armed": true,
  "running": true,
  "failsafe": false,
  "motor1_target": 45,
  "motor1_applied": 42,
  "motor2_target": 45,
  "motor2_applied": 42,
  "cooling_requested": true,
  "pump": true,
  "tec": true,
  "clients": 1,
  "ip": "192.168.4.1"
}
```

Details: **[firmware/README.md](firmware/README.md)**.

---

## 15. Safety requirements

Rotating machinery above a person makes safety a **core** requirement.

**Mechanical:** full prop guards, rigid mounts, vibration-resistant fasteners, balanced frame, secondary tether, keep net fabric / hair / fingers out of the disc.

**Electrical:** AC fuse, master switch, proper mains enclosure/earthing as applicable, correct wire gauge, strain relief, secure connectors; AC section stays off the hanging assembly.

**Software:** motors off at boot; controlled ramp; heartbeat failsafe; no auto-restart after disconnect; cooling interlocks; reject invalid API input.

**Physical emergency disconnect** is strongly recommended. Software STOP is **not** the only emergency path.

---

## 16. Testing procedure

### Stage A — no propellers

**Remove both propellers** before any ESC test.

1. ESP32 boot, AP, phone join, `http://192.168.4.1`  
2. Captive-portal behavior where the OS supports it  
3. ESC arming (3 s), START at 0%, Motor 1 / Motor 2 independently  
4. STOP, Wi-Fi drop → failsafe within ~1.5 s, no auto-restart  
5. Cooling: pump before TEC; TEC off when Motor 1 low/stopped  

### Stage B — loaded (only after Stage A)

Propellers + guards; measure current, temperatures, vibration, noise, airflow; long-duration runs; cooling thermal and condensation checks.

**Firmware compile success ≠ hardware validation.**

---

## 17. Project status

**Stage:** Hardware architecture / **prototype development**

Repository includes concept docs, cooling diagrams, PlatformIO firmware (modular SoftAP UI, dual ESC, ramp, failsafe, cooling sequence), and CI build workflow.

| Claim type | Status |
|------------|--------|
| Firmware designed & modularized | Yes |
| Firmware compile verification | **Pending CI result after current PWM compatibility fix** |
| Physical motor/cooling validation | **Not complete** — pending bench & field tests |

Do **not** mark hardware validation complete without measurements.

---

## 18. Development roadmap

**Firmware (implemented in repo):**

- [x] ESP32-C3 Wi-Fi SoftAP (`AirNode-Duo` / `airnode123` → 192.168.4.1)  
- [x] Local web UI + captive-portal DNS/redirects  
- [x] Dual ESC PWM, independent + master speed  
- [x] Controlled startup ramp, explicit START/STOP  
- [x] Heartbeat failsafe (1.5 s; no auto-restart)  
- [x] Cooling sequence (pump prime → TEC; Motor 1 airflow interlock)  
- [x] Modular `src/` layout
- [x] Arduino-ESP32 3.x-compatible LEDC ESC output (14-bit at 50 Hz)
- [x] Arduino-ESP32 2.x compatibility path retained  

**Next / hardware:**

- [ ] Captive-portal polish on more phone OS versions  
- [ ] Current / voltage sensing  
- [ ] Motor, ESC, TEC hot-side, coolant temperature  
- [ ] Coolant flow / pump-fail detection  
- [ ] Live telemetry (only with real sensors)  
- [ ] Stronger cooling safety state machine  
- [ ] Physical emergency-stop input  
- [ ] Prop guards, frame, long-duration thermal/vibration tests  
- [ ] Propeller selection from measured data  

---

## 19. Engineering rules (summary)

- Do not invent measurements or claim untested hardware capability  
- Do not treat 600 W PSU rating as actual system consumption  
- Do not equate PWM % with motor current  
- Do not claim the Peltier air-conditions the tent  
- Do not drive high-current loads from ESP32 GPIO  
- Do not auto-start motors after boot or after failsafe/reconnect  
- Do not enable TEC without pump-prime + airflow logic  
- Core control must work **without** internet or cloud  

---

## 20. Verification status

The repository is ready for the next software/bench-test stage, but physical operation is still unverified.

### Resolved in the current firmware

- Updated ESC PWM handling for Arduino-ESP32 3.x (ledcAttach / pin-based ledcWrite).
- Retained a compatibility path for Arduino-ESP32 2.x.
- Changed ESP32-C3 ESC PWM resolution from 16-bit to **14-bit**, matching the current Arduino-ESP32 LEDC range for ESP32-C3.
- Added a PWM-initialization failure lockout so the controller cannot arm motors if LEDC setup fails.

### Still requires physical hardware validation

- Exact ESP32-C3 board pinout.
- ESC signal/arming behavior with the selected LittleBee ESCs.
- Motor/propeller current and temperature.
- Propeller direction and downward airflow.
- Frame strength, vibration and guards.
- Peltier hot-side temperature.
- Coolant flow and leak resistance.
- Condensation behavior.
- Physical emergency disconnect.

**A successful firmware build does not prove that the motor/cooling system is safe for operation above a person.**

## 20. Documentation map

| Path | Role |
|------|------|
| **[README.md](README.md)** (this file) | Full project context, architecture, safety, roadmap |
| **[firmware/README.md](firmware/README.md)** | Build, flash, API, GPIO, bench checklist |
| [docs/project-concept.md](docs/project-concept.md) | Concept notes |
| [hardware/README.md](hardware/README.md) | Hardware notes |
| [docs/](docs/) / [hardware/](hardware/) | Diagrams and prototype media |

---

## 22. Intended end-to-end experience

```text
USER POWERS AIRNODE DUO
        │
        ▼
ESP32-C3 BOOTS  →  motors OFF, TEC OFF, pump OFF
        │
        ▼
Wi-Fi AP "AirNode-Duo"
        │
        ▼
PHONE CONNECTS  →  captive portal and/or http://192.168.4.1
        │
        ▼
CONTROL PAGE  →  motor + cooling UI
        │
        ├── START + sliders  →  ramped dual airflow
        └── COOLING ON       →  pump → TEC (if Motor 1 OK)
        │
        ▼
DOWNWARD AIRFLOW  →  PERSON

If phone lost → 1.5 s timeout → FAILSAFE (motors + cooling OFF)
                → wait for user START again
```

---

## License

Choose and declare a license that matches the firmware, CAD, and documentation you intend to publish.
