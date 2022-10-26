#include "Arduino.h"

#include "SequencePianoRollState.h"

SequencePianoRollState::SequencePianoRollState(BuitStateMachine& buitMachine, BuitDevicesManager& devices) : BuitState(devices), _buitMachine(buitMachine) {
  Serial.println("SequencePianoRollState");
  _buitMachine = buitMachine;
}

void SequencePianoRollState::singleClick() {
  //Serial.println("Does nothing here!");
}

void SequencePianoRollState::doubleClick() {
  Serial.println("Going to Sequence Edit!");
  _devices.printToScreen("State:", "Sequence Edit!","");
  _buitMachine.setState(_buitMachine.getSequenceEditState());
}

void SequencePianoRollState::longClick() {
  //Serial.println("Does nothing here!");
}

void SequencePianoRollState::rotaryTurned(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void SequencePianoRollState::threeAxisChanged(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void SequencePianoRollState::trellisPressed(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void SequencePianoRollState::trellisReleased(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void SequencePianoRollState::sequencerCallback(ControlCommand command) {
  //Serial.println("Does nothing here!");
}