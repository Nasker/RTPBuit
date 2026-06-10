#include "Arduino.h"

#include "SceneEditState.h"
#include "constants.h"

SceneEditState::SceneEditState (BuitStateMachine& buitMachine, BuitDevicesManager& devices) : BuitState(devices), _buitMachine(buitMachine) {
  Serial.println("SceneEditState");
}

void SceneEditState::onEnter() {
  _devices.presentScene();
}

void SceneEditState::singleClick() {
  _buitMachine.setState(_buitMachine.getSequenceSelectState());
}

void SceneEditState::doubleClick() {
  _buitMachine.setState(_buitMachine.getTransportState());
}

void SceneEditState::tripleClick() {
 // Serial.println("Does nothing here!");
}

void SceneEditState::longClick() {
  _buitMachine.setState(_buitMachine.getSceneSettingsState());
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
  // Handle transport state changes to update display
  if (command.commandType == TRANSPORT_START || command.commandType == TRANSPORT_STOP) {
    _devices.presentScene();
  }
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