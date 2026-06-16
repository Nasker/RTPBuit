#include "Arduino.h"

#include "GlobalSettingsState.h"

GlobalSettingsState::GlobalSettingsState (BuitStateMachine& buitMachine, BuitDevicesManager& devices) : BuitState(devices), _buitMachine(buitMachine) {
  Serial.println("GlobalSettingsState");
  _buitMachine = buitMachine;
}

void GlobalSettingsState::onEnter() {
  _devices.printToScreen("Global Settings", "", "");
}

void GlobalSettingsState::singleClick() {
  //Serial.println("Does nothing here!");
}

void GlobalSettingsState::doubleClick() {
  _buitMachine.setState(_buitMachine.getTransportState());
}

void GlobalSettingsState::tripleClick() {
  //Serial.println("Does nothing here!");
}

void GlobalSettingsState::longClick() {
  //Serial.println("Does nothing here!");
}

void GlobalSettingsState::rotaryTurned(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void GlobalSettingsState::threeAxisChanged(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void  GlobalSettingsState::trellisPressed(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void GlobalSettingsState::trellisReleased(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void GlobalSettingsState::sequencerCallback(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void GlobalSettingsState::midiNote(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void GlobalSettingsState::midiNoteOff(ControlCommand command) {
  // Simple implementation - just ignore note-off events
  // No need to do anything here
}

void GlobalSettingsState::midiCC(ControlCommand command) {
  //Serial.println("Does nothing here!");
}