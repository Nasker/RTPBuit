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
  _devices.printToScreen("State:", "Sequence Edit!","");
  _buitMachine.setState(_buitMachine.getSequenceEditState());
}

void SequenceSettingsState::longClick() {
  //Serial.println("Does nothing here!");
}

void SequenceSettingsState::rotaryTurned(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void SequenceSettingsState::threeAxisChanged(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void SequenceSettingsState::trellisPressed(ControlCommand command) {
  //Serial.println("Does nothing here!");
} 

void SequenceSettingsState::trellisReleased(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void SequenceSettingsState::sequencerCallback(ControlCommand command) {
  //Serial.println("Does nothing here!");
}