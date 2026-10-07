# AirNode Duo — Center control / cooling enclosure

Low-profile 3D-printed box for the **balanced center node** on the mosquito-net / tent structure.

## Files (print-ready)

| File | Role | Bounding box (mm) | Mesh notes |
|------|------|-------------------|------------|
| `case-body.stl` | Main body | **130 × 110 × 50** | **Open top** for lid; cleaned for FDM |
| `cap.stl` | Lid | **130 × 110 × 15** | Watertight; print flat |

## 3D print settings (recommended)

| Setting | Body | Lid |
|---------|------|-----|
| Material | **PETG** | PETG |
| Nozzle | 0.4 mm | 0.4 mm |
| Layer height | **0.20 mm** | 0.20 mm |
| Walls / perimeters | **3** | 3 |
| Top/bottom layers | 4–5 | 4–5 |
| Infill | **15–25%** gyroid or grid | 15–25% |
| Supports | **None** (correct orientation) | None |
| Orientation | **Open face UP**, floor on bed | Flat on bed |

### Orientation

```text
case-body.stl                 cap.stl
   open top ↑
   ┌─────────┐               ┌─────────┐
   │  cavity │               │   lid   │
   │  floor  │ ← on bed      └─────────┘ ← on bed
   └─────────┘
```

### Printability

- Body is an **open-top shell** (not a sealed hollow bubble) so FDM slices cleanly.
- Floor at Z = 0 for bed contact.
- After print: drill **Ø8–9 mm** side holes for RF-370 / block tubing if needed.
- Cut or design a **side grille** for TEC heatsink fan exhaust (hot air out, not into the net).

### Fit

| Item | Guidance |
|------|----------|
| Internal height | ~47–48 mm above floor |
| Pump RF-370 | ~84 mm × Ø29 — along 130 mm axis |
| TEC stack | **Flat**; fan exhaust **sideways** |
| Lid | Light friction or M3 corner screws |

## Contents

| Component | Spec |
|-----------|------|
| ESP32-C3 + buck 12→5 V | Dry side |
| TEC1-12706 | 40×40×~3.8 mm; MOSFET only (~4–6 A) |
| Water block | 40×40×(12+7) mm |
| Heatsink + fan | Compact TEC kit |
| Pump | RF-370CA-12560, Ø8 mm ports |

## Safety

- TEC/pump via MOSFET — never direct ESP32 GPIO.
- Leak-test before enabling TEC.
- Secondary tether on hanging assembly.

## Related

- Motor frames: [`../frames/`](../frames/)
- Hardware overview: [`../README.md`](../README.md)
