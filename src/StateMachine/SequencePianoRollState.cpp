#include "Arduino.h"
#include "SequencePianoRollState.h"
#include "constants.h"

SequencePianoRollState::SequencePianoRollState(BuitStateMachine& buitMachine, BuitDevicesManager& devices) : BuitState(devices), _buitMachine(buitMachine) {
  Serial.println("SequencePianoRollState");
  _buitMachine = buitMachine;
}

void SequencePianoRollState::onEnter() {
  _devices.paintLiveTrellis();

  uint8_t seqType = _devices.getSelectedSequenceType();
  String typeName;
  switch (seqType) {
    case DRUM_PART:  typeName = "Drum";  break;
    case BASS_SYNTH: typeName = "Bass";  break;
    case MONO_SYNTH: typeName = "Lead";  break;
    case POLY_SYNTH: typeName = "Poly";  break;
    default:         typeName = "Live";  break;
  }
  _devices.printToScreen("Piano Roll", typeName, "Live");
}

void SequencePianoRollState::singleClick() {}

void SequencePianoRollState::doubleClick() {
  _buitMachine.setState(_buitMachine.getSequenceEditState());
}

void SequencePianoRollState::tripleClick() {
  _devices.toggleSelectedSequenceRecording();
}

void SequencePianoRollState::longClick() {}

void SequencePianoRollState::rotaryTurned(ControlCommand command) {
  //_devices.nudgePage(command);
}

void SequencePianoRollState::threeAxisChanged(ControlCommand command) {
  uint8_t seqType = _devices.getSelectedSequenceType();
  if (seqType == DRUM_PART)
    _devices.handleLiveDrumRollThreeAxis(command);
  else
    _devices.handleLiveThreeAxis(command);
}

void SequencePianoRollState::trellisPressed(ControlCommand command) {
  _devices.handleLiveTrellisPressed((uint8_t)command.value);
}

void SequencePianoRollState::trellisReleased(ControlCommand command) {
  _devices.handleLiveTrellisReleased((uint8_t)command.value);
}

void SequencePianoRollState::sequencerCallback(ControlCommand command) {
  if (command.commandType == GRID_TICK)
    _devices.handleLiveSequencerTick();
  if (command.commandType == GRID_FINE_TICK)
    _devices.handleLiveFineTick();
  if (_devices.isSelectedSequenceRecording())
    _devices.displayCursorInSequence(command);
}

void SequencePianoRollState::midiNote(ControlCommand command) {}

void SequencePianoRollState::midiNoteOff(ControlCommand command) {}

void SequencePianoRollState::midiCC(ControlCommand command) {}