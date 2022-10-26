#include "Arduino.h"

#include "SequenceEditState.h"

SequenceEditState::SequenceEditState(BuitStateMachine& buitMachine, BuitDevicesManager& devices) : BuitState(devices), _buitMachine(buitMachine) {
  Serial.println("SequenceEditState");
  _buitMachine = buitMachine;
}

void SequenceEditState::singleClick() {
  //Serial.println("Does nothing here!");
}

void SequenceEditState::doubleClick() {
  Serial.println("Going to Scene Edit!");
  _devices.presentScene();
  _buitMachine.setState(_buitMachine.getSceneEditState());
}

void SequenceEditState::longClick() {
  //Serial.println("Going to Sequence Settings!");
  _devices.printToScreen("State:", "Sequence Settings!","");
  _buitMachine.setState(_buitMachine.getSequenceSettingsState());
}

void SequenceEditState::rotaryTurned(ControlCommand command) {
  _devices.nudgePage(command);
}

void SequenceEditState::threeAxisChanged(ControlCommand command) {
  _devices.editCurrentNote(command);
}

void SequenceEditState::trellisPressed(ControlCommand command) {
  _devices.editSequence(command);
}

void SequenceEditState::trellisReleased(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void SequenceEditState::sequencerCallback(ControlCommand command) {
  _devices.displayCursorInSequence(command);
}