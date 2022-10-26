#include "Arduino.h"

#include "GlobalSettingsState.h"

GlobalSettingsState::GlobalSettingsState (BuitStateMachine& buitMachine, BuitDevicesManager& devices) : BuitState(devices), _buitMachine(buitMachine) {
  Serial.println("GlobalSettingsState");
  _buitMachine = buitMachine;
}

void GlobalSettingsState::singleClick() {
  //Serial.println("Does nothing here!");
}

void GlobalSettingsState::doubleClick() {
  Serial.println("Going to Transport");
  _devices.printToScreen("Transport", "", "");
  _buitMachine.setState(_buitMachine.getTransportState());
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