#include "Arduino.h"

#include "SequenceEditState.h"

SequenceEditState::SequenceEditState(BuitStateMachine& buitMachine, BuitDevicesManager& devices) : BuitState(devices), _buitMachine(buitMachine) {
  Serial.println("SequenceEditState");
}

void SequenceEditState::onEnter() {
  _devices.showSequence();
}

void SequenceEditState::singleClick() {
  _buitMachine.setState(_buitMachine.getSequencePianoRollState());
}

void SequenceEditState::doubleClick() {
  _buitMachine.setState(_buitMachine.getSceneEditState());
}

void SequenceEditState::tripleClick() {
  Serial.println("Toggling Sequence Recording!");
  _devices.toggleSelectedSequenceRecording();
}

void SequenceEditState::longClick() {
  _buitMachine.setState(_buitMachine.getSequenceSettingsState());
}

void SequenceEditState::rotaryTurned(ControlCommand command) {
  //Serial.println("Rotary turned: " + String(command.commandType));
  _devices.nudgePage(command);
}

void SequenceEditState::threeAxisChanged(ControlCommand command) {
  if(_devices.isSelectedSequenceRecording())
    _devices.editCurrentNote(command);
}

void SequenceEditState::trellisPressed(ControlCommand command) {
  _devices.editSequence(command);
  _devices.showSequence();
}

void SequenceEditState::trellisReleased(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void SequenceEditState::sequencerCallback(ControlCommand command) {
  _devices.displayCursorInSequence(command);
}

void SequenceEditState::midiNote(ControlCommand command) {
  // Always play the note via MIDI
  int midiChannel = _devices.getSelectedSequenceMidichannel();
  usbMIDI.sendNoteOn(command.commandType, command.value, midiChannel);
  
  // Record via NotesRecorder when recording is active
  if(_devices.isSelectedSequenceRecording()) {
    _devices.recorderNoteOn(command.commandType, command.value);
  }
}

void SequenceEditState::midiNoteOff(ControlCommand command) {
  // Send MIDI note-off message
  int midiChannel = _devices.getSelectedSequenceMidichannel();
  usbMIDI.sendNoteOff(command.commandType, 0, midiChannel);
  
  if(_devices.isSelectedSequenceRecording()) {
    _devices.recorderNoteOff(command.commandType);
  }
}

void SequenceEditState::midiCC(ControlCommand command) {
  _devices.editCurrentNote(command);
} 