#include "Arduino.h"

#include "SequenceSettingsState.h"

SequenceSettingsState::SequenceSettingsState(BuitStateMachine& buitMachine, BuitDevicesManager& devices) : BuitState(devices), _buitMachine(buitMachine) {
  Serial.println("SequenceSettingsState");
}

void SequenceSettingsState::onEnter() {
  _devices.discardParameterEdit();  // safety: no stale staged value on entry
  _devices.presentSequenceSettings();
}

void SequenceSettingsState::singleClick() {
  // Press = commit the staged value to the selected parameter.
  _devices.commitParameterEdit();
  _devices.presentSequenceSettings();
}

void SequenceSettingsState::doubleClick() {
  _devices.discardParameterEdit();  // leaving without pressing = not applied
  _buitMachine.setState(_buitMachine.getSequenceEditState());
}

void SequenceSettingsState::tripleClick() {
  //Serial.println("Does nothing here!");
}

void SequenceSettingsState::longClick() {
  // Hold toggles back out of settings (mirrors SequenceEdit's long-press to enter).
  _devices.discardParameterEdit();
  _buitMachine.setState(_buitMachine.getSequenceEditState());
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

void SequenceSettingsState::midiNoteOff(ControlCommand command) {
  // Simple implementation - just ignore note-off events
  // No need to do anything here
}

void SequenceSettingsState::midiCC(ControlCommand command) {
  //Serial.println("Does nothing here!");
}