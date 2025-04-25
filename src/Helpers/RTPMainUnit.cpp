#include "RTPMainUnit.hpp"
#include "ControlCommand.h"

RTPMainUnit::RTPMainUnit(){
}

void RTPMainUnit::begin(){  
  Serial.begin(9600);
  Wire.begin();
  Wire1.begin();
  vlSensor.initSetup();
  vlSensor.startContinuous();
  rtpTrellis.begin(this);
  SequencerManager.begin(this);
  devicesManager.initSetup();
}

void RTPMainUnit::update(){
  rtpRotary.callbackFromRotary(this);
  rtpRotary.callbackFromClicks(this);
  rtpTrellis.read();
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
  stateMachineManager.handleActions(callbackCommand);
}

void RTPMainUnit::linkToSequencerManager(byte realtimebyte){
  SequencerManager.handleRealTimeSystem(realtimebyte);
}

void RTPMainUnit::routeControlChange(byte channel, byte control, byte value) {
  musicManager.setCurrentHarmony(channel, control, value);
  ControlCommand command = ControlCommand{MIDI_CC, control, value};
  stateMachineManager.handleActions(command);
}

void RTPMainUnit::routeNoteOnOff(byte channel, byte note, byte velocity){
  // Create a control command with different control types for note-on and note-off
  // For note-on: controlType = MIDI_NOTE (7)
  // For note-off: controlType = MIDI_NOTE + 100 (107)
  int controlType = (velocity > 0) ? MIDI_NOTE : MIDI_NOTE + 100;
  
  ControlCommand command = ControlCommand{controlType, note, velocity};
  stateMachineManager.handleActions(command, channel);
}