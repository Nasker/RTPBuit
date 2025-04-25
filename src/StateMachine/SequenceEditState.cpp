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
  _buitMachine.setState(_buitMachine.getSceneEditState());
  _devices.presentScene();
}

void SequenceEditState::tripleClick() {
  Serial.println("Toggling Sequence Recording!");
  _devices.toggleSelectedSequenceRecording();
}

void SequenceEditState::longClick() {
  Serial.println("Going to Sequence Settings!");
  _buitMachine.setState(_buitMachine.getSequenceSettingsState());
  _devices.presentSequenceSettings();
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
  
  // Only record the note if recording is active
  if(_devices.isSelectedSequenceRecording()) {
    // Edit the note at the current sequencer position
    // This allows for real-time note recording quantized to 1/16th notes
    _devices.editCurrentNote(command);
    
    // Store the note-on time for calculating note length later
    // This would be implemented if we want to track note lengths
  }
}

void SequenceEditState::midiNoteOff(ControlCommand command) {
  // Send MIDI note-off message
  int midiChannel = _devices.getSelectedSequenceMidichannel();
  usbMIDI.sendNoteOff(command.commandType, 0, midiChannel);
  
  // If recording is active, we could calculate the note length here
  // For now, we'll just use the default note length based on sequence type
}

void SequenceEditState::midiCC(ControlCommand command) {
  _devices.editCurrentNote(command);
} 