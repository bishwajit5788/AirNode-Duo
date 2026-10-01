#pragma once

#include <Arduino.h>
#include "config.h"

namespace CoolingControl {

void begin();
void update(bool systemRunning, uint8_t motor1AppliedPct);

bool isRequested();
bool isPumpOn();
bool isTecOn();
bool isPriming();  // pump on, TEC not yet on

void setRequested(bool on);
void forceOff();   // STOP / failsafe path

}  // namespace CoolingControl
