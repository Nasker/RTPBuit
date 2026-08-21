#pragma once

#include "Arduino.h"
#include <Wire.h>
#include "Hardware/Implementations/TeensyMidiOutput.hpp"
#include "Hardware/Adapters/RTPOledAdapter.hpp"
#include "Hardware/Adapters/RTPNeoTrellisAdapter.hpp"
#include "Hardware/Adapters/RTPRotaryAdapter.hpp"
#include "Hardware/Adapters/RTPThreeAxisAdapter.hpp"
#include "Managers/DeviceManager.hpp"
#include <memory>
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
#include "Midi/MidiRouter.hpp"
#include "Midi/UsbDeviceMidiOutput.hpp"
#include "Midi/DinMidiOutput.hpp"
#include "Midi/InternalMidiSink.hpp"
#include "Midi/UsbHostMidiOutput.hpp"
#include "Midi/UsbHostManager.hpp"
  
class RTPMainUnit{
  // Legacy hardware (kept for backward compatibility)
  RTPOled rtpOled;
  RTPNeoTrellis rtpTrellis;
  RTPRotaryClickDev rtpRotary{ROT_LEFT_PIN, ROT_RIGHT_PIN, BUTTON_PIN, LOW, true};
  RTPThreeAxisVL vlSensor;
  
  // Core components
  MusicManager musicManager;
  RTPSequencer Sequencer{MusicConfig::Sequences::N_SCENES, musicManager};
  RTPClockGenerator clockGenerator;
  TeensyMidiOutput midiOutput;
  
  // MIDI Router and per-port outputs
  MidiRouter midiRouter;
  UsbDeviceMidiOutput usbDeviceOutput;
  UsbHostMidiOutput usbHostOutput;
  DinMidiOutput dinOutput;
  InternalMidiSink internalSink;
  UsbHostManager usbHostManager;
  
  // Hardware adapters (bridge legacy to interfaces)
  RTPOledAdapter oledAdapter{rtpOled};
  RTPNeoTrellisAdapter trellisAdapter{rtpTrellis};
  RTPRotaryAdapter rotaryAdapter{rtpRotary};
  RTPThreeAxisAdapter sensorAdapter{vlSensor};
  
  // Shared pointers for DeviceManager
  std::shared_ptr<IDisplay> displayPtr;
  std::shared_ptr<IButtonMatrix> buttonMatrixPtr;
  std::shared_ptr<IRotaryEncoder> rotaryPtr;
  std::shared_ptr<IThreeAxisSensor> sensorPtr;
  std::shared_ptr<IMidiOutput> midiOutputPtr;
  std::shared_ptr<IClockGenerator> clockGenPtr;
  std::shared_ptr<ISequencer> sequencerPtr;
  
  // Modern device manager (composition of focused managers)
  std::unique_ptr<DeviceManager> deviceManager;
  
  // Legacy facade (interface-based, shared adapters with DeviceManager)
  BuitDevicesManager devicesManager{oledAdapter, trellisAdapter, Sequencer};
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
  void routeControlChange(uint8_t channel, uint8_t control, uint8_t value,
                           uint8_t srcPort = 0, uint8_t srcDevice = 0xFF);
  void routeNoteOnOff(uint8_t channel, uint8_t note, uint8_t velocity,
                      uint8_t srcPort = 0, uint8_t srcDevice = 0xFF);
  MidiRouter& getMidiRouter() { return midiRouter; }
  UsbHostManager& getUsbHostManager() { return usbHostManager; }
  void setUsbHostDevice(MIDIDevice* device, uint8_t idx = 0);
  void setClockOutputPorts(MidiPort destMask) { midiRouter.setClockOutputPorts(destMask); }
  MidiPort getClockOutputPorts() const { return midiRouter.getClockOutputPorts(); }
  void setClockInputSource(MidiPort sourceMask) { midiRouter.setClockInputSource(sourceMask); }
  MidiPort getClockInputSource() const { return midiRouter.getClockInputSource(); }
private:
  void initMidiRouter();
};
