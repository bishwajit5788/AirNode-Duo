# AirNode Duo — 3D-printed motor frames

Printed propeller guards / motor cages for the dual-node AirNode Duo prototype.

Motors mount **inside** the frames. Each node is a **bottom cage + top grille**.

## Files

| File | Role | Bounding box (mm) | Triangles |
|------|------|-------------------|-----------|
| `b-frame-61mm.stl` | **Motor 2** bottom cage (air-only, no liquid coil) | **253.47 × 253.47 × 61.0** | 240 288 |
| `b-frame-71mm-cooled.stl` | **Motor 1** bottom cage (space for copper cooling coil under motor) | **253.47 × 253.47 × 71.0** | 240 288 |
| `t-frame.stl` | Top grille (pair with either bottom) | **253.47 × 253.47 × 4.54** | 102 108 |
| `t-frame-2.stl` | Second top grille (print one per node) | **253.47 × 253.47 × 4.54** | 102 108 |

## Which frame for which motor

```text
Motor 1 (cooled path)          Motor 2 (normal airflow)
┌─────────────────────┐        ┌─────────────────────┐
│ t-frame*.stl        │        │ t-frame*.stl        │
│ 9450 prop           │        │ 9450 prop           │
│ 2312 motor          │        │ 2312 motor          │
│ copper coil ~20 mm  │        │ (no coil)           │
│ b-frame-71mm-cooled │        │ b-frame-61mm        │
└─────────────────────┘        └─────────────────────┘
     Z = 71 mm                      Z = 61 mm
     X = Y ≈ 253.5 mm               X = Y ≈ 253.5 mm
```

| Node | Bottom STL | Top STL | Z height |
|------|------------|---------|----------|
| **Motor 1** (Peltier liquid coil under motor) | `b-frame-71mm-cooled.stl` | `t-frame.stl` or `t-frame-2.stl` | **71.0 mm** |
| **Motor 2** (no coil) | `b-frame-61mm.stl` | `t-frame.stl` or `t-frame-2.stl` | **61.0 mm** |

Height difference (**+10 mm** on Motor 1) is for the self-made thin copper cooling coil (~15–20 mm pack) under the motor inside the cage.

## Propeller compatibility

| Item | Value |
|------|--------|
| Propeller | **9450** carbon fiber (F450/F550 class), CW |
| Prop disc | ~**9.4″ ≈ 239 mm** tip-to-tip |
| Frame outer | ~**253.5 mm** |
| Typical wall | ~3–4 mm → inner clear ~**245–247 mm** |
| Fit | **Usable** if prop is centered; measure real prop and inner Ø before high throttle |

## Motor compatibility

| Item | Value |
|------|--------|
| Motor | Readytosky **2312 920KV** CW |
| Body height | **26.0 mm** (datasheet) |
| Body diameter | **27.7 mm** |
| Mount | **12 × 16 mm** M3 pattern — verify against base plate holes |

## Bill of printed parts (one complete dual-motor set)

- 1 × `b-frame-71mm-cooled.stl` (Motor 1)
- 1 × `b-frame-61mm.stl` (Motor 2)
- 2 × top grille (`t-frame.stl` + `t-frame-2.stl`)

## Suggested print settings (Bambu Lab A1 reference)

| Setting | Suggestion |
|---------|------------|
| Material | **PETG** (toughness for guards) |
| Nozzle | 0.4 mm |
| Layer height | 0.20 mm |
| Walls | 3 loops |
| Infill | 15–25% (tri-hexagon or gyroid) |
| Supports | As needed for the side windows / overhangs |
| Orientation | Base flat on bed (Z up) |

Always dry-fit **motor + prop (and coil on Motor 1)** before final assembly.

## Assembly notes

1. Mount the **2312** to the center of the **bottom** cage (motor inside).
2. Install **9450** on the motor shaft (correct CW direction for downward airflow).
3. On Motor 1 only: place the **copper coil** under the motor / in the extra Z space; route inlet/outlet through a side opening — do not fully block the prop disc.
4. Fit the **top grille**; ensure prop tips clear the inner wall and grille.
5. Route ESC/signal wires out a side window with strain relief.
6. Use a **secondary safety tether** on the hanging assembly.

## Safety

- First electrical tests: **propellers removed**.
- First spinning tests: low throttle, guards on, no body over the disc.
- These frames are **guards**, not a substitute for careful mounting and current measurement.

## Source

Derived from a freely available fan-cover / 风扇罩 style model, scaled in Z per node:

- Motor 2 air-only: **Z = 61 mm**
- Motor 1 cooled: **Z = 71 mm**
- X/Y held at model outer ~**253.5 mm**

Original concept dimensions and AirNode Duo system docs: see [`../README.md`](../README.md) and the repository root `README.md`.
