#include "Arduino.h"

#include "SceneSettingsState.h"
#include "PatternBankState.h"

SceneSettingsState::SceneSettingsState (BuitStateMachine& buitMachine, BuitDevicesManager& devices) : BuitState(devices), _buitMachine(buitMachine) {
  Serial.println("SceneSettingsState");
  _buitMachine = buitMachine;
}

void SceneSettingsState::singleClick() {
  //Serial.println("Does nothing here!");
}

void SceneSettingsState::doubleClick() {
  Serial.println("Going to Scene Edit!");
  _buitMachine.setState(_buitMachine.getSceneEditState());
  _devices.presentScene();
}

void SceneSettingsState::tripleClick() {
  PatternBankState* bankState = static_cast<PatternBankState*>(_buitMachine.getPatternBankLoadState());
  bankState->enter();
  _buitMachine.setState(bankState);
}

void SceneSettingsState::longClick() {
  PatternBankState* bankState = static_cast<PatternBankState*>(_buitMachine.getPatternBankSaveState());
  bankState->enter();
  _buitMachine.setState(bankState);
}

void SceneSettingsState::rotaryTurned(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void SceneSettingsState::threeAxisChanged(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void SceneSettingsState::trellisPressed(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void SceneSettingsState::trellisReleased(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void SceneSettingsState::sequencerCallback(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void SceneSettingsState::midiNote(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void SceneSettingsState::midiNoteOff(ControlCommand command) {
  // Simple implementation - just ignore note-off events
  // No need to do anything here
}

void SceneSettingsState::midiCC(ControlCommand command) {
  //Serial.println("Does nothing here!");
}