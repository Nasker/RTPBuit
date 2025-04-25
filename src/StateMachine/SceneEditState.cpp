#include "Arduino.h"

#include "SceneEditState.h"

SceneEditState::SceneEditState (BuitStateMachine& buitMachine, BuitDevicesManager& devices) : BuitState(devices), _buitMachine(buitMachine) {
  Serial.println("SceneEditState");
}

void SceneEditState::singleClick() {
  Serial.println("Going to Sequence Select!");
  _buitMachine.setState(_buitMachine.getSequenceSelectState());
  _devices.printToScreen("Select a", "Sequence", "");
}

void SceneEditState::doubleClick() {
  Serial.println("Going to Transport");
  _buitMachine.setState(_buitMachine.getTransportState());
  _devices.presentBuitCC();
}

void SceneEditState::tripleClick() {
 // Serial.println("Does nothing here!");
}

void SceneEditState::longClick() {
  Serial.println("Going to Scene Settings!");
  _buitMachine.setState(_buitMachine.getSceneSettingsState());
  _devices.printToScreen("Scene Settings", "","");
}

void SceneEditState::rotaryTurned(ControlCommand command) {
  _devices.changeScene(command);
}

void SceneEditState::threeAxisChanged(ControlCommand command) {
  _devices.sendBuitCC(command);
}

void SceneEditState::trellisPressed(ControlCommand command) { 
  _devices.editScene(command);
}

void SceneEditState::trellisReleased(ControlCommand command) {
}

void SceneEditState::sequencerCallback(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void SceneEditState::midiNote(ControlCommand command) {
  //usbMIDI.sendNoteOn(command.commandType, command.value, command.controlType);
}

void SceneEditState::midiNoteOff(ControlCommand command) {
  // Simple implementation - just ignore note-off events
  // No need to do anything here
}

void SceneEditState::midiCC(ControlCommand command) {
  //Serial.println("Does nothing here!");
}