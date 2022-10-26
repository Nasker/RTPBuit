#pragma once

#include "Arduino.h"
#include <Wire.h>
#include "RTPNeoTrellis.hpp"
#include "RTPRotaryClickChordion.hpp"
#include "RTPThreeAxisVL.hpp"
#include "ChordionKeys.hpp"
#include "BuitDevicesManager.hpp"
#include "StateMachineManager.hpp"
#include "RTPSequencer.h"
#include "RTPSequencerManager.hpp"
#include "MusicManager.hpp"
#include "constants.h"
  
class RTPMainUnit{
  RTPNeoTrellis rtpTrellis;
  RTPRotaryClickDev rtpRotary{ROT_LEFT_PIN, ROT_RIGHT_PIN, BUTTON_PIN, LOW, true};
  RTPThreeAxisVL vlSensor;
  MusicManager musicManager;
  RTPSequencer Sequencer{N_SCENES, musicManager};
  BuitDevicesManager devicesManager{rtpTrellis, Sequencer};
  StateMachineManager stateMachineManager{devicesManager};
  RTPSequencerManager SequencerManager{Sequencer};
  
public:
  RTPMainUnit();
  void begin();
  void update();
  void updatePeriodically();
  void linkToSequencerManager(byte realtimebyte);
  void actOnThreeAxisCallback(String callbackString, int rangeValue);
  void actOnControlsCallback(ControlCommand answer);
  void actOnSequencerCallback(ControlCommand answer);
  void routeControlChange(byte channel, byte control, byte value);
};
