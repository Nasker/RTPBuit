#include "Arduino.h"
#include "TransportState.h"

TransportState::TransportState (BuitStateMachine& buitMachine, BuitDevicesManager& devices) : BuitState(devices), _buitMachine(buitMachine) {
  Serial.println("TransportState");
  _buitMachine = buitMachine;
}

void TransportState::singleClick() {
}

void TransportState::doubleClick() {
  Serial.println("Going to Scene Edit!");
  _buitMachine.setState(_buitMachine.getSceneEditState());
  _devices.presentScene();
}

void TransportState::tripleClick() {
  //Serial.println("Does nothing here!");
}

void TransportState::longClick() {
  Serial.println("Going to Global Settings!");
  _buitMachine.setState(_buitMachine.getGlobalSettingsState());
  _devices.printToScreen("Global Settings", "","");
}

void TransportState::rotaryTurned(ControlCommand command) {

}

void TransportState::threeAxisChanged(ControlCommand command) {
  _devices.sendBuitCC(command);
}

void TransportState::trellisPressed(ControlCommand command) {
  _devices.editBuitCC(command);
}

void TransportState::trellisReleased(ControlCommand command) {

}  

void TransportState::sequencerCallback(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void TransportState::midiNote(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void TransportState::midiNoteOff(ControlCommand command) {
  // Simple implementation - just ignore note-off events
  // No need to do anything here
}

void TransportState::midiCC(ControlCommand command) {
  //Serial.println("Does nothing here!");
}