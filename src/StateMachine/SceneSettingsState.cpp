#include "Arduino.h"

#include "SceneSettingsState.h"

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
  _devices.printToScreen("Load Data", "LoadingData","");
  _devices.loadSequencer("scenes.json");
}

void SceneSettingsState::longClick() {
  _devices.printToScreen("Dump Data", "DumpingData","");
  _devices.saveSequencer("scenes.json");
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

void SceneSettingsState::midiCC(ControlCommand command) {
  //Serial.println("Does nothing here!");
}