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
  _devices.printToScreen("State:", "Scene Edit!","");
  _buitMachine.setState(_buitMachine.getSceneEditState());
}

void SceneSettingsState::longClick() {
  _devices.printToScreen("Dump Data", "DumpingData","");
  _devices.dumpSequencesToJson();
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