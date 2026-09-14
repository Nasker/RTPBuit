#pragma once

#include "Arduino.h"

struct ControlCommand{
  int controlType;
  int commandType;
  int value;
  uint8_t sourcePort    = 0;   // MidiPort as uint8_t (0 = NONE / internal)
  uint8_t sourceDevice  = 0xFF; // USB Host device index (0xFF = any/unknown)
};
