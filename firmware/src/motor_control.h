#pragma once

#include <Arduino.h>
#include "config.h"

namespace MotorControl {

void begin();
void update();  // call from loop: arming + ramp

bool isArmed();
bool isRunning();
bool isFailsafe();

uint8_t motor1Target();
uint8_t motor2Target();
uint8_t motor1Applied();
uint8_t motor2Applied();

void setTargets(uint8_t m1, uint8_t m2);
void start();   // requires armed; does not auto-set targets
void stop();    // immediate hard stop of motors (cooling handled separately)
void triggerFailsafe();

void touchControl();  // refresh heartbeat timestamp
bool hasRecentControl();

}  // namespace MotorControl
