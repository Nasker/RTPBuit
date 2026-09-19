#include "Arduino.h"

#include "SequenceSelectState.h"

SequenceSelectState::SequenceSelectState(BuitStateMachine& buitMachine, BuitDevicesManager& devices) : BuitState(devices), _buitMachine(buitMachine) {
  Serial.println("SequenceSelectState");
}

void SequenceSelectState::onEnter() {
  _devices.printToScreen("Select a", "Sequence", "");
  _devices.presentSequenceSelect();
}

void SequenceSelectState::singleClick() {
  _buitMachine.setState(_buitMachine.getSequenceEditState());
}


void SequenceSelectState::doubleClick() {
  _buitMachine.setState(_buitMachine.getSceneEditState());
}

void SequenceSelectState::tripleClick() {
  //Serial.println("Does nothing here!");
}

void SequenceSelectState::longClick() {
  //Serial.println("Does nothing here!");
}

void SequenceSelectState::rotaryTurned(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void SequenceSelectState::threeAxisChanged(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void SequenceSelectState::trellisPressed(ControlCommand command) {
  Serial.printf("Editing sequence #%d\n", command.value);
  _devices.selectSequence(command);
  _buitMachine.setState(_buitMachine.getSequenceEditState());
}

void SequenceSelectState::trellisReleased(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void SequenceSelectState::sequencerCallback(ControlCommand command) {
  // Keep the sounding-pad flash alive while picking a sequence — same
  // repaint SceneEditState performs on fine ticks and transport changes.
  if (command.commandType == TRANSPORT_START || command.commandType == TRANSPORT_STOP) {
    _devices.presentSequenceSelect();
  }
  else if (command.commandType == GRID_FINE_TICK) {
    _devices.refreshSceneGrid();
  }
}

void SequenceSelectState::midiNote(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void SequenceSelectState::midiNoteOff(ControlCommand command) {
  // Simple implementation - just ignore note-off events
  // No need to do anything here
}

void SequenceSelectState::midiCC(ControlCommand command) {
  //Serial.println("Does nothing here!");
}