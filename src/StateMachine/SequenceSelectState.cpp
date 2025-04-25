#include "Arduino.h"

#include "SequenceSelectState.h"

SequenceSelectState::SequenceSelectState(BuitStateMachine& buitMachine, BuitDevicesManager& devices) : BuitState(devices), _buitMachine(buitMachine) {
  Serial.println("SequenceSelectState");
  _buitMachine = buitMachine;
}

void SequenceSelectState::singleClick() {
  Serial.println("Going to Sequence Edit!");
  _buitMachine.setState(_buitMachine.getSequenceEditState());
  _devices.showSequence();
}


void SequenceSelectState::doubleClick() {
  Serial.println("Going to Scene Edit!");
  _buitMachine.setState(_buitMachine.getSceneEditState());
  _devices.presentScene();
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
  _devices.showSequence();
}

void SequenceSelectState::trellisReleased(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void SequenceSelectState::sequencerCallback(ControlCommand command) {
  //Serial.println("Does nothing here!");
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