#include "Arduino.h"

#include "SequenceEditState.h"
#include "Midi/MidiRouter.hpp"
#include "Midi/MidiMessage.hpp"

MidiRouter* SequenceEditState::_router = nullptr;

void SequenceEditState::setRouter(MidiRouter* router) {
    _router = router;
}

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
  // Always play the note via MIDI router
  int midiChannel = _devices.getSelectedSequenceMidichannel();
  if (_router) {
    MidiMessage msg { MidiMessage::NoteOn,
                      static_cast<uint8_t>(midiChannel),
                      static_cast<uint8_t>(command.commandType),
                      static_cast<uint8_t>(command.value),
                      MidiPort::INTERNAL };
    _router->route(msg);
  }
  
  // Record via NotesRecorder when recording is active
  if(_devices.isSelectedSequenceRecording()) {
    _devices.recorderNoteOn(command.commandType, command.value);
  }
}

void SequenceEditState::midiNoteOff(ControlCommand command) {
  // Send MIDI note-off via router
  int midiChannel = _devices.getSelectedSequenceMidichannel();
  if (_router) {
    MidiMessage msg { MidiMessage::NoteOff,
                      static_cast<uint8_t>(midiChannel),
                      static_cast<uint8_t>(command.commandType),
                      0,
                      MidiPort::INTERNAL };
    _router->route(msg);
  }
  
  if(_devices.isSelectedSequenceRecording()) {
    _devices.recorderNoteOff(command.commandType);
  }
}

void SequenceEditState::midiCC(ControlCommand command) {
  _devices.editCurrentNote(command);
} 