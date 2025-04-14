#pragma once

#include <Arduino.h>
#include "RTPRotaryClick.h"
#include "ControlCommand.h"

class RTPMainUnit;
class RTPRotaryClickDev: public RTPRotaryClick{
public:
  RTPRotaryClickDev(int leftRotPin, int rightRotPin, int buttonPin, boolean active, boolean pullup):RTPRotaryClick(leftRotPin, rightRotPin, buttonPin, active, pullup){};
  void callbackFromRotary(RTPMainUnit*);
  void callbackFromClicks(RTPMainUnit*);
};