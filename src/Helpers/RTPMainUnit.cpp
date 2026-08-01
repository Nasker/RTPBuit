#include "RTPMainUnit.hpp"
#include "ControlCommand.h"

RTPMainUnit::RTPMainUnit(){
  // Create shared pointers to adapters for DeviceManager
  // Using shared_ptr with custom no-op deleter since adapters are stack objects
  displayPtr = std::shared_ptr<IDisplay>(&oledAdapter, [](IDisplay*){});
  buttonMatrixPtr = std::shared_ptr<IButtonMatrix>(&trellisAdapter, [](IButtonMatrix*){});
  rotaryPtr = std::shared_ptr<IRotaryEncoder>(&rotaryAdapter, [](IRotaryEncoder*){});
  sensorPtr = std::shared_ptr<IThreeAxisSensor>(&sensorAdapter, [](IThreeAxisSensor*){});
  midiOutputPtr = std::shared_ptr<IMidiOutput>(&midiOutput, [](IMidiOutput*){});
  clockGenPtr = std::shared_ptr<IClockGenerator>(&clockGenerator, [](IClockGenerator*){});
  sequencerPtr = std::shared_ptr<ISequencer>(&Sequencer, [](ISequencer*){});
  
  // Create DeviceManager with all adapters
  deviceManager = std::make_unique<DeviceManager>(
    displayPtr,
    buttonMatrixPtr,
    rotaryPtr,
    sensorPtr,
    clockGenPtr,
    sequencerPtr,
    midiOutputPtr
  );
}

void RTPMainUnit::begin(){  
  Serial.begin(9600);
  Wire.begin();
  Wire1.begin();
  
  // Initialize hardware
  rtpOled.init();
  Sequencer.setMidiOutput(&midiOutput);
  vlSensor.initSetup();
  vlSensor.startContinuous();
  rtpTrellis.begin(this);
  
  // Initialize modern DeviceManager
  if (deviceManager) {
    auto result = deviceManager->initialize();
    if (result.isError()) {
      Serial.println("ERROR: DeviceManager initialization failed");
    } else {
      Serial.println("DeviceManager initialized successfully");
    }
  }
  
  // Initialize legacy managers
  SequencerManager.begin(this);
  SequencerManager.setClockGenerator(clockGenerator);
  devicesManager.initSetup();
  devicesManager.setClockGenerator(clockGenerator);
}

void RTPMainUnit::update(){
  rtpRotary.callbackFromRotary(this);
  rtpRotary.callbackFromClicks(this);
  rtpTrellis.read();
  SequencerManager.update();
}

void RTPMainUnit::updatePeriodically(){
  vlSensor.callbackThreeAxisChanged(this);
}

void RTPMainUnit::actOnControlsCallback(ControlCommand callbackCommand){
  //Serial.printf("DEVICE: %d  TYPE: %d  VALUE: %d\n", callbackCommand.controlType, callbackCommand.commandType, callbackCommand.value);
  stateMachineManager.handleActions(callbackCommand);
}

void RTPMainUnit::actOnSequencerCallback(ControlCommand callbackCommand){
  //Serial.printf("Seq Callback  TYPE: %d  VALUE: %d\n", callbackCommand.commandType, callbackCommand.value);
  devicesManager.recorderAdvanceTick();
  stateMachineManager.handleActions(callbackCommand);
}

void RTPMainUnit::linkToSequencerManager(uint8_t realtimebyte){
  // Only process external MIDI when in External mode
  if (clockGenerator.getMode() == SyncMode::External) {
    SequencerManager.handleRealTimeSystem(realtimebyte);
  }
}

void RTPMainUnit::routeControlChange(uint8_t channel, uint8_t control, uint8_t value) {
  musicManager.setCurrentHarmony(channel, control, value);
  ControlCommand command = ControlCommand{MIDI_CC, control, value};
  stateMachineManager.handleActions(command);
}

void RTPMainUnit::routeNoteOnOff(uint8_t channel, uint8_t note, uint8_t velocity){
  // Create a control command with different control types for note-on and note-off
  // For note-on: controlType = MIDI_NOTE (7)
  // For note-off: controlType = MIDI_NOTE + 100 (107)
  int controlType = (velocity > 0) ? MIDI_NOTE : MIDI_NOTE + 100;
  
  ControlCommand command = ControlCommand{controlType, note, velocity};
  stateMachineManager.handleActions(command, channel);
}