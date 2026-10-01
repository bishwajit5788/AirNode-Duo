# AirNode Duo — Project Concept

## Motto

> **Cool the person, not the room — compact airflow where it matters.**

## Physical arrangement

AirNode Duo uses two roof-mounted BLDC rotors positioned over the sleeping area. Both rotors are intended to produce **downward airflow**, creating a ceiling-fan-like circulation pattern.

The heavy AC-to-DC SMPS remains on the floor/table or another protected location. The roof assembly carries only the lightweight motors, ESCs, ESP32-C3, buck converter, wiring and safety hardware.

```text
                 ROOF OF MOSQUITO NET / TENT
        ──────────────────────────────────────────

              ┌───────────┐     ┌───────────┐
              │ 2312 920KV│     │ 2312 920KV│
              │  BLDC #1  │     │  BLDC #2  │
              └─────┬─────┘     └─────┬─────┘
                    │                   │
                ┌───▼───┐           ┌───▼───┐
                │  PROP  │           │  PROP  │
                └───┬───┘           └───┬───┘
                    ↓↓↓                 ↓↓↓
                 ↓↓↓↓↓↓↓             ↓↓↓↓↓↓↓
                    ↓↓↓                 ↓↓↓
                                      /
                                     /
                                    /
                          ▼▼▼▼▼▼▼▼▼
                         PERSON / BED

        ══════════════════════════════════════════
               12 V DC CABLE FROM OUTSIDE
                          │
                   ┌──────▼──────┐
                   │ 12V/50A/600W│
                   │     SMPS    │
                   └─────────────┘
                         230 V AC
```


## Motor 1 thermoelectric cooling module

The current concept adds a **12 V / ~2 A Peltier cooling stage to Motor 1**.

The Peltier does not sit directly in the propeller airflow. Instead:

1. The Peltier cold side cools a liquid through a proper cold-side thermal block.
2. A small pump circulates the coolant through flexible plastic tubing.
3. The coolant passes through a **copper tube coil positioned behind Motor 1's propeller**.
4. Motor 1 forces air across the copper coil.
5. The resulting cooled airflow continues downward toward the occupant.
6. The Peltier hot side is cooled separately with an aluminium heatsink and fan, with hot air directed away from the occupant.

```text
             MOTOR 1
          2312 920KV
               │
             9450
               ↓↓↓
        ┌───────────────┐
        │ COPPER COIL   │  ← coolant from pump
        └───────┬───────┘
                ↓↓↓
          COOLED AIRFLOW
                ↓↓↓
             PERSON

     PELTIER → COLD BLOCK → PUMP → COIL
        │
        └→ HOT SIDE → ALUMINIUM HEATSINK + FAN → HOT AIR OUT
```

The copper coil is therefore a **liquid-to-air heat exchanger**. It should not cover the entire propeller disc because excessive airflow blockage can reduce airflow and increase motor load.

A proper cold-side plate/water block and thermal interface material are required between the Peltier and coolant. If the proposed "1 mm copper tube" refers to internal diameter, a larger coolant passage should be considered because a very small passage can create excessive pressure drop.

Because the cold coil may reach temperatures below ambient dew point, condensation must be considered. A sealed coolant reservoir, leak-resistant fittings and temperature monitoring are recommended before mounting the system above a person.

## Design intent

The purpose is not to cool the whole room. The system creates a controlled local airflow stream across the occupant, which can improve perceived thermal comfort through increased convective heat transfer and evaporation.

The final system should prioritize:
- low mass on the net/tent roof;
- balanced and vibration-resistant mounting;
- controlled continuous airflow;
- independent rotor control;
- reliable shutdown on communication loss;
- physical rotor and electrical protection.

## Important design note

The two motors are both CW variants. Rotor direction and propeller installation must be verified experimentally so **both rotors produce the intended downward airflow**. Motor label convention alone does not establish airflow direction.

The hardware documentation is now maintained in `hardware/README.md`, with `hardware/concept/` reserved for clean concept diagrams and `hardware/prototype/` reserved for dated real-hardware photographs. Future prototype photos should document the frame, motors, propellers, copper coil, Peltier assembly, pump, wiring, controller and complete system.


## Current integrated hardware concept

The project has now evolved from a dual-airflow prototype into a **dual-airflow + Motor 1 thermoelectric cooling prototype**.

### Motor 1 — cooled airflow

**2312 920KV CW motor → 9450-class propeller → copper coolant coil → cooled downward airflow → occupant.**

The copper coil is a liquid-to-air heat exchanger. Coolant is circulated by a small pump through flexible tubing. The Peltier cools the coolant through a proper cold-side copper/aluminium block.

### Motor 2 — normal airflow

**2312 920KV CW motor → 9450-class propeller → unobstructed downward airflow → occupant.**

This provides independent airflow even when the thermoelectric stage is disabled.

### Thermal rejection

The Peltier hot side uses a dedicated aluminium heatsink and cooling fan. Hot air must be exhausted away from the occupant and away from the cooled coil. The Peltier's electrical input becomes additional heat that the hot side must reject, so heatsink capacity is a critical part of the design.

### Safety and validation priorities

- Use full propeller guards and a secondary safety tether.
- Keep the 230 V AC SMPS outside the hanging assembly.
- Use a sealed coolant reservoir and leak-resistant fittings.
- Add hot-side and cold-side/coolant temperature monitoring.
- Disable the Peltier if the hot side becomes too hot or coolant flow fails.
- Check for condensation before operation above a person.
- Do not allow the copper coil to excessively obstruct the propeller.
- Measure actual motor current with the final propeller before defining continuous operating limits.

### Hardware documentation structure

The repository now uses `hardware/README.md` as the central hardware file. It is intended to grow with the project and will contain future dated photographs and test documentation as the physical hardware arrives.

Current concept illustration:
`hardware/concept/airnode-duo-thermoelectric-cooling.svg`

Future real photographs:
`hardware/prototype/YYYY-MM-DD_description.jpg`
