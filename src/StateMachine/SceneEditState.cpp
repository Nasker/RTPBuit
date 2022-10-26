#include "Arduino.h"

#include "SceneEditState.h"

SceneEditState::SceneEditState (BuitStateMachine& buitMachine, BuitDevicesManager& devices) : BuitState(devices), _buitMachine(buitMachine) {
  Serial.println("SceneEditState");
}

void SceneEditState::singleClick() {
  Serial.println("Going to Sequence Select!");
  _devices.printToScreen("Sequence Select", "", "");
  _buitMachine.setState(_buitMachine.getSequenceSelectState());
}

void SceneEditState::doubleClick() {
  Serial.println("Going to Transport");
  _devices.presentBuitCC();
  _buitMachine.setState(_buitMachine.getTransportState());
}

void SceneEditState::longClick() {
  Serial.println("Going to Scene Settings!");
  _devices.printToScreen("Scene Settings", "","");
  _buitMachine.setState(_buitMachine.getSceneSettingsState());
}

void SceneEditState::rotaryTurned(ControlCommand command) {
  //Serial.printf("Rotary turned: %d\n", command.commandType);
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