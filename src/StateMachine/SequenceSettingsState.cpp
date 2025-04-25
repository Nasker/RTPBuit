#include "Arduino.h"

#include "SequenceSettingsState.h"

SequenceSettingsState::SequenceSettingsState(BuitStateMachine& buitMachine, BuitDevicesManager& devices) : BuitState(devices), _buitMachine(buitMachine) {
  Serial.println("SequenceSettingsState");
  _buitMachine = buitMachine;
}

void SequenceSettingsState::singleClick() {
  //Serial.println("Does nothing here!");
}

void SequenceSettingsState::doubleClick() {
  Serial.println("Going to Sequence Edit!");
  _buitMachine.setState(_buitMachine.getSequenceEditState());
  _devices.showSequence();
}

void SequenceSettingsState::tripleClick() {
  //Serial.println("Does nothing here!");
}

void SequenceSettingsState::longClick() {
  //Serial.println("Does nothing here!");
}

void SequenceSettingsState::rotaryTurned(ControlCommand command) {
  _devices.rotateParameter(command);
  _devices.presentSequenceSettings();
}

void SequenceSettingsState::threeAxisChanged(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void SequenceSettingsState::trellisPressed(ControlCommand command) {
  Serial.printf("Select Parameter: %d\n", command.value);
  _devices.selectParameter(command);
  _devices.presentSequenceSettings();
} 

void SequenceSettingsState::trellisReleased(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void SequenceSettingsState::sequencerCallback(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void SequenceSettingsState::midiNote(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void SequenceSettingsState::midiCC(ControlCommand command) {
  //Serial.println("Does nothing here!");
}