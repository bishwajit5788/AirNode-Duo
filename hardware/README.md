# AirNode Duo — Hardware

This directory is the central hardware documentation area for the AirNode Duo prototype.

## Purpose

AirNode Duo is a lightweight, roof-mounted, Wi-Fi-controlled dual-BLDC airflow system for a mosquito net / compact sleeping space.

The current prototype concept combines:

- **Motor 1:** 2312 920KV CW BLDC + 9350-class CW propeller
- **Motor 2:** 2312 920KV CW BLDC + 9350-class CW propeller
- **Motor 1 cooling stage:** 12 V / ~2 A Peltier + liquid coolant loop + copper tube coil behind the propeller
- **Motor 2:** normal unobstructed airflow
- **ESCs:** 2 × Favorite LittleBee 30A-S OPTO
- **Controller:** ESP32-C3
- **Main supply:** external 12 V / 50 A / 600 W AC-DC SMPS
- **Logic supply:** 12 V → 5 V buck converter
- **Cooling loop:** Peltier cold block → pump → flexible tube → copper coil → return
- **Hot side:** aluminium heatsink + dedicated cooling fan

## Hardware layout

The intended arrangement is:

```text
                         MOSQUITO-NET ROOF
────────────────────────────────────────────────────────

        MOTOR 1                                  MOTOR 2
      2312 920KV                               2312 920KV
           │                                        │
         9350                                     9350
           │                                        │
           ▼                                        ▼
   ┌───────────────┐                         NORMAL AIRFLOW
   │ COPPER COIL   │                               ↓↓↓
   │ HEAT EXCHANGER│                               ↓↓↓
   └───────┬───────┘                               ↓↓↓
           ↓
      COOLED AIRFLOW
           ↓
        OCCUPANT

        PELTIER
           │
      COLD-SIDE BLOCK
           │
        COOLANT
           │
          PUMP
           │
     PLASTIC TUBE
           │
     COPPER COIL
           │
         RETURN

    HOT SIDE → ALUMINIUM HEATSINK + FAN → HOT AIR AWAY
```

## Hardware specification

| Subsystem | Component | Specification / role |
|---|---|---|
| Airflow | 2 × BLDC motor | Readytosky 2312 920KV |
| Propeller | 2 × | 9350-class CW; current/temp to be measured with guards |
| Motor ESC | 2 × | Favorite LittleBee 30A-S OPTO |
| Controller | 1 × | ESP32-C3, Wi-Fi web control |
| Main PSU | 1 × | 12 V / 50 A / 600 W external SMPS |
| Logic PSU | 1 × | 12 V → 5 V buck converter |
| TEC | 1 × | 12 V / ~2 A Peltier class |
| Cold-side exchanger | 1 × | Copper/aluminium water block |
| Liquid pump | 1 × | Small 12 V pump |
| Air heat exchanger | 1 × | Copper tube coil behind Motor 1 propeller |
| Tubing | As required | Flexible coolant tubing; size to be selected from pump/flow testing |
| Hot-side cooling | 1 × | Aluminium heatsink + 12 V cooling fan |
| Reservoir | 1 × | Sealed coolant reservoir |
| Sensors | Recommended | Hot-side, cold-side/coolant temperature; optional flow sensor |
| Safety | Required | Prop guards, fuse/protection, master disconnect, secondary safety tether |

## Photo archive

This folder is intentionally structured as a **living hardware archive**.

Add future prototype photographs here using descriptive filenames:

```text
hardware/
├── README.md
├── concept/
│   └── airnode-duo-minimal-diagram.jpg
├── prototype/
│   ├── 2026-XX-XX_frame.jpg
│   ├── 2026-XX-XX_motor-1.jpg
│   ├── 2026-XX-XX_motor-2.jpg
│   ├── 2026-XX-XX_peltier-loop.jpg
│   ├── 2026-XX-XX_copper-coil.jpg
│   ├── 2026-XX-XX_control-electronics.jpg
│   └── 2026-XX-XX_complete-prototype.jpg
└── testing/
    ├── airflow-test/
    ├── thermal-test/
    └── electrical-test/
```

### Photo naming convention

Use:

```text
YYYY-MM-DD_component_or_view_description.jpg
```

Examples:

- `2026-10-05_complete-prototype.jpg`
- `2026-10-05_motor-1-copper-coil.jpg`
- `2026-10-05_peltier-cold-block.jpg`
- `2026-10-05_hot-side-heatsink.jpg`
- `2026-10-05_esp32-esc-wiring.jpg`

For every real hardware photograph, add a short caption containing:

1. What is shown
2. Component/model
3. Purpose
4. Relevant specification
5. Prototype revision/date
6. Any important modification

## Important prototype notes

- The copper coil is a **liquid-to-air heat exchanger** positioned behind Motor 1's propeller.
- The Peltier must be thermally coupled to a proper cold-side block; do not rely on direct contact with the tube.
- The hot side must continuously reject heat through an adequately sized heatsink and fan.
- The coil must not excessively obstruct the propeller.
- If the proposed 1 mm copper tube means **1 mm internal diameter**, re-evaluate it because the pressure drop may be excessive for a small pump.
- Use a sealed reservoir and leak-resistant fittings because the assembly is positioned above the occupant.
- Condensation must be checked before continuous operation.
- Actual motor current must be measured with the final propeller before selecting operating limits.
- Software control is not the only safety mechanism: use physical electrical and mechanical protection.

## Current status

**Stage:** Concept / hardware preparation

**Next hardware milestone:** assemble and photograph the first physical frame, cooling loop and electrical/control assembly. Replace concept illustrations with real prototype photographs as the hardware becomes available.

## Current concept image

The current project concept image is stored directly in the hardware archive:

![AirNode Duo hardware concept](prototype/airnode-duo-overview.jpg)

This image represents the **current design concept**, not a photograph of the assembled physical prototype. Future real hardware photographs should be added under `hardware/prototype/` with dated filenames and captions.

## Concept diagrams

- [Thermoelectric cooling (SVG)](concept/airnode-duo-thermoelectric-cooling.svg) — schematic diagram
- [Thermoelectric cooling (PNG)](concept/airnode-duo-thermoelectric-cooling.png) — generated concept illustration
- [Thermoelectric cooling (JPG)](concept/airnode-duo-thermoelectric-cooling.jpg) — same illustration


## Propeller guards (selected free frame)

| Parameter | Value |
|-----------|--------|
| Source | Freely available fan cover / 风扇罩 frame |
| Overall size | **~251 × 251 × 75 mm** |
| Propeller | **9350-class** (~9.3″ / ~236 mm) |
| Fit note | Confirm real prop diameter and center bolt pattern before printing two units |

Print suggestion: PETG, 0.2 mm layer, verify hub/motor mount compatibility.
