#pragma once

#include <VL53L0X.h>
#include "Wire.h"
#include "ControlCommand.h"
#include "constants.h"
#include "Structs.h"

#define DEFAULT_MAX_READING 306
#define DEFAULT_MIN_READING 50

class RTPMainUnit;

class RTPThreeAxisVL{
  VL53L0X sensor1;
  VL53L0X sensor2;
  VL53L0X sensor3;
  ThreeReadings lastReadings;
  int _maxLimitReading;
  int _minLimitReading;
public:
  RTPThreeAxisVL();
  void initSetup();
  void startContinuous();
  void stopContinuous();
  void setMaxLimitReading(int maxReading);
  int getMaxLimitReading();
  void setMinLimitReading(int minReading);
  int getMinLimitReading();
  ThreeReadings getThreeReadings();
  ThreeReadings getThreeCleanReadings();
  void callbackThreeAxisChanged(void (*userFunc) (ControlCommand callbackCommand));
  void callbackThreeAxisChanged(RTPMainUnit*);
};
