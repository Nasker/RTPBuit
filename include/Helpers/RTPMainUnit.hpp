#pragma once

#include "Arduino.h"
#include <Wire.h>
#include "Hardware/Implementations/TeensyMidiOutput.hpp"
#include "Hardware/Adapters/RTPOledAdapter.hpp"
#include "Hardware/Adapters/RTPNeoTrellisAdapter.hpp"
#include "Hardware/Adapters/RTPRotaryAdapter.hpp"
#include "Hardware/Adapters/RTPThreeAxisAdapter.hpp"
#include "RTPOled.hpp"
#include "RTPNeoTrellis.hpp"
#include "RTPRotaryClickChordion.hpp"
#include "RTPThreeAxisVL.hpp"
#include "ChordionKeys.hpp"
#include "BuitDevicesManager.hpp"
#include "StateMachineManager.hpp"
#include "RTPSequencer.h"
#include "RTPSequencerManager.hpp"
#include "MusicManager.hpp"
#include "Helpers/RTPClockGenerator.hpp"
#include "constants.h"
  
class RTPMainUnit{
  // Legacy hardware (kept for backward compatibility)
  RTPOled rtpOled;
  RTPNeoTrellis rtpTrellis;
  RTPRotaryClickDev rtpRotary{ROT_LEFT_PIN, ROT_RIGHT_PIN, BUTTON_PIN, LOW, true};
  RTPThreeAxisVL vlSensor;
  
  // Core components
  MusicManager musicManager;
  RTPSequencer Sequencer{N_SCENES, musicManager};
  RTPClockGenerator clockGenerator;
  TeensyMidiOutput midiOutput;
  
  // Hardware adapters (bridge legacy to interfaces) - initialized in constructor
  RTPOledAdapter oledAdapter{rtpOled};
  RTPNeoTrellisAdapter trellisAdapter{rtpTrellis};
  RTPRotaryAdapter rotaryAdapter{rtpRotary};
  RTPThreeAxisAdapter sensorAdapter{vlSensor};
  
  // Managers (legacy god object still in use)
  BuitDevicesManager devicesManager{rtpTrellis, Sequencer};
  StateMachineManager stateMachineManager{devicesManager};
  RTPSequencerManager SequencerManager{Sequencer};
  
public:
  RTPMainUnit();
  void begin();
  void update();
  void updatePeriodically();
  void linkToSequencerManager(uint8_t realtimebyte);
  void actOnThreeAxisCallback(String callbackString, int rangeValue);
  void actOnControlsCallback(ControlCommand answer);
  void actOnSequencerCallback(ControlCommand answer);
  void routeControlChange(uint8_t channel, uint8_t control, uint8_t value);
  void routeNoteOnOff(uint8_t channel, uint8_t note, uint8_t velocity);
};
