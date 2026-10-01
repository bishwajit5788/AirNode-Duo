#pragma once

#include <Arduino.h>
#include <IPAddress.h>
#include "config.h"

namespace WifiManager {

void begin();
void process();  // DNS

IPAddress apIp();
uint8_t clientCount();

}  // namespace WifiManager
