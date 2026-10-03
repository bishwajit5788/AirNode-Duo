#pragma once

#include <Arduino.h>
#include "config.h"

namespace CoolingControl {

void begin();
void update(bool systemRunning, uint8_t motor1AppliedPct);

// Requested cooling level 0..100. 0 = off. >0 = cooling requested (TEC is
// binary on this prototype after pump prime — level is not PWM power).
uint8_t level();
bool isRequested();  // level() > 0
bool isPumpOn();
bool isTecOn();
bool isPriming();    // pump on, TEC not yet on, request active

void setLevel(uint8_t pct);  // 0..100
void setRequested(bool on);  // compatibility: on -> 100, off -> 0
void forceOff();             // STOP / failsafe path

}  // namespace CoolingControl
