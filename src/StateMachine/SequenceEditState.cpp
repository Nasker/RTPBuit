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
  // Leaving this sequence's edit context — stop any active/armed recording
  // (dumps what was captured) so it can't leak into the scene level.
  _devices.stopSelectedSequenceRecording();
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
  // A finished take returns to the view it was armed from — if that was the
  // piano roll, leave the sequence edit now that recording is done.
  if (_devices.consumeRecordReturnView() == 1) {
    _buitMachine.setState(_buitMachine.getSequencePianoRollState());
    return;
  }
  _devices.displayCursorInSequence(command);
}

void SequenceEditState::midiNote(ControlCommand command) {
  // Thru and recording are handled by MidiInputDispatcher before the event
  // reaches the UI — nothing view-specific left to do here.
}

void SequenceEditState::midiNoteOff(ControlCommand command) {
  // See midiNote — dispatcher owns thru + record.
}

void SequenceEditState::midiCC(ControlCommand command) {
  if (!_devices.acceptsInputFrom(command.sourcePort, command.sourceDevice)) return;
  _devices.editCurrentNote(command);
} 