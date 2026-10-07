# AirNode Duo — Center control / cooling enclosure

Low-profile 3D-printed box for the **balanced center node** that sits on the mosquito-net / tent structure between the two motor frames.

Houses the ESP32-C3 controller, TEC cooling stack, pump, and drivers — kept **short in height** so the assembly does not stand high on soft fabric.

## Files

| File | Role | Bounding box (mm) |
|------|------|-------------------|
| `case-body.stl` | Main enclosure body | **130.0 × 110.0 × 50.0** |
| `cap.stl` | Lid / cover | **130.0 × 110.0 × 15.0** |

| Assembled | Approx. |
|-----------|---------|
| Footprint | **130 × 110 mm** |
| Body height | **50 mm** |
| Lid | **15 mm** (may overlap rim → external height often ~50–55 mm) |

## Intended contents

| Component | Spec / notes |
|-----------|----------------|
| ESP32-C3 | SoftAP controller + buck 12 V → 5 V |
| Peltier | **TEC1-12706** (40 × 40 × ~3.8 mm), ~12 V, **~4–6 A** — MOSFET driver only |
| Cold block | Aluminium water block **40 × 40 × (12+7) mm** |
| Hot side | Heatsink **with fan** kit for TEC1-12706 (compact, ~40 mm-class fan) |
| Pump | **RF-370CA-12560** 12 V self-priming diaphragm (~**65 + 19 mm** long, **Ø ~29 mm**, **Ø 8 mm** ports) |
| Drivers | Logic-level MOSFETs for pump (GPIO 6) and TEC (GPIO 7) |

## Layout (low height)

Lay the TEC sandwich **flat**; exhaust the hot fan **out the side** (not into the net):

```text
        ←———— 130 mm ————→
┌────────────────────────────┐
│ ESP32 + buck + FETs (dry)  │
│ RF-370 pump (along length) │  110 mm
│ [block | TEC | HS+fan] → side exhaust
└────────────────────────────┘
         height 50 mm body
```

## Fit check

| Part | Fits? |
|------|--------|
| Pump ~84 mm long | Yes, along 130 mm axis |
| Pump Ø29 mm | Yes, inside ~47–48 mm internal height |
| 40×40 block + TEC + HS/fan **flat** | Yes, if HS+fan pack height ≤ ~40–45 mm |
| Upright TEC stack | Too tall for 50 mm body — **do not use** |

## Print suggestions

| Setting | Suggestion |
|---------|------------|
| Material | PETG |
| Layer height | 0.20 mm |
| Walls | 3 |
| Infill | 20–30% |
| Orientation | Body open face up; lid flat |

Add **side vents** for the heatsink fan (cut or design grille). Keep coolant fittings and electronics separated; plan a drip path that does not wet the ESP32.

## Electrical / safety

- TEC1-12706 needs a **≥ 10 A** MOSFET path — never direct ESP32 GPIO.
- Firmware: pump prime before TEC; Motor 1 airflow interlock (see firmware docs).
- Leak-test the loop before enabling the Peltier.
- Secondary **tether** the whole hanging assembly.

## Related

- Motor frames: [`../frames/`](../frames/)
- Hardware overview: [`../README.md`](../README.md)
