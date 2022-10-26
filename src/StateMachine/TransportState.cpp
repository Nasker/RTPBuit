#include "Arduino.h"
#include "TransportState.h"

TransportState::TransportState (BuitStateMachine& buitMachine, BuitDevicesManager& devices) : BuitState(devices), _buitMachine(buitMachine) {
  Serial.println("TransportState");
  _buitMachine = buitMachine;
}

void TransportState::singleClick() {
}

void TransportState::doubleClick() {
  Serial.println("Going to Scene Edit!");
  _devices.presentScene();
  _buitMachine.setState(_buitMachine.getSceneEditState());
}

void TransportState::longClick() {
  Serial.println("Going to Global Settings!");
  _devices.printToScreen("Global Settings", "","");
  _buitMachine.setState(_buitMachine.getGlobalSettingsState());
}

void TransportState::rotaryTurned(ControlCommand command) {

}

void TransportState::threeAxisChanged(ControlCommand command) {
  _devices.sendBuitCC(command);
}

void TransportState::trellisPressed(ControlCommand command) {
  _devices.editBuitCC(command);
}

void TransportState::trellisReleased(ControlCommand command) {

}  

void TransportState::sequencerCallback(ControlCommand command) {
  //Serial.println("Does nothing here!");
}