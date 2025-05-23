#include "Arduino.h"
#include "InitState.h"

InitState::InitState(BuitStateMachine& buitMachine, BuitDevicesManager& devices) : BuitState(devices), _buitMachine(buitMachine) {
  Serial.println("InitState");
}

void InitState::singleClick() {
  Serial.println("Going to Transport");
  _devices.presentBuitCC();
  _buitMachine.setState(_buitMachine.getTransportState());
}

void InitState::doubleClick() {
  Serial.println("Going to Transport");
  _devices.presentBuitCC();
  _buitMachine.setState(_buitMachine.getTransportState());
}

void InitState::tripleClick() {
 // Serial.println("Does nothing here!");
}

void InitState::longClick() {
  Serial.println("Going to Transport");
  _devices.presentBuitCC();    
  _buitMachine.setState(_buitMachine.getTransportState());
}

void InitState::rotaryTurned(ControlCommand command) {
  Serial.println("Going to Transport");
  _devices.presentBuitCC();
  _buitMachine.setState(_buitMachine.getTransportState());
}

void InitState::threeAxisChanged(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void InitState::trellisPressed(ControlCommand command) {
  Serial.println("Going to Transport");
  _devices.presentBuitCC();
  _buitMachine.setState(_buitMachine.getTransportState());
} 

void InitState::trellisReleased(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void InitState::sequencerCallback(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void InitState::midiNote(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void InitState::midiNoteOff(ControlCommand command) {
  // Simple implementation - just ignore note-off events
  // No need to do anything here
}

void InitState::midiCC(ControlCommand command) {
  //Serial.println("Does nothing here!");
}