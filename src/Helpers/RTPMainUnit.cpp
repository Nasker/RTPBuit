#include "RTPMainUnit.hpp"
#include "ControlCommand.h"

RTPMainUnit::RTPMainUnit(){
}

void RTPMainUnit::begin(){  
  Serial.begin(9600);
  Wire.begin();
  Wire1.begin();
  Sequencer.setMidiOutput(&midiOutput);
  vlSensor.initSetup();
  vlSensor.startContinuous();
  rtpTrellis.begin(this);
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