#include "BassSequence.hpp"
#include "ReMap.hpp"

BassSequence::BassSequence(int midiChannel, int NEvents, int type, int baseNote, NotesPlayer& notesPlayer, MusicManager& musicManager):
  RTPEventNoteSequence(midiChannel, NEvents, type, baseNote, notesPlayer, musicManager){
  setTypeSpecificColor();
}

void BassSequence::setTypeSpecificColor(){
    setColor(BASS_SYNTH_COLOR_IDX);
}

void BassSequence::playLiveNoteOn(uint8_t rootNote, uint8_t velocity, uint8_t chordType) {
    (void)velocity;
    uint8_t ch = getMidiChannel();
    while (!_liveRingingNotes.empty()) {
        uint8_t note = _liveRingingNotes.front();
        usbMIDI.sendNoteOff(note, 0, ch);
        Serial1.write(0x80 | ((ch - 1) & 0x0F));
        Serial1.write(note & 0x7F);
        Serial1.write(0x00);
        _liveRingingNotes.pop();
    }
    _musicManager.setChordType(chordType);
    uint8_t steps = _musicManager.getChordSteps();
    uint8_t transposed = rootNote + (_liveOctave * 12);
    for (uint8_t i = 0; i < steps; i++) {
        int interval = _musicManager.getChordStep(i);
        if (interval <= 0) {
            uint8_t note = transposed + interval;
            usbMIDI.sendNoteOn(note, _liveVelocity, ch);
            Serial1.write(0x90 | ((ch - 1) & 0x0F));
            Serial1.write(note & 0x7F);
            Serial1.write(_liveVelocity & 0x7F);
            _liveRingingNotes.push(note);
        }
    }
}

void BassSequence::playLiveNoteOff(uint8_t rootNote, uint8_t chordType) {
    (void)rootNote;
    (void)chordType;
    uint8_t ch = getMidiChannel();
    while (!_liveRingingNotes.empty()) {
        uint8_t note = _liveRingingNotes.front();
        usbMIDI.sendNoteOff(note, 0, ch);
        Serial1.write(0x80 | ((ch - 1) & 0x0F));
        Serial1.write(note & 0x7F);
        Serial1.write(0x00);
        _liveRingingNotes.pop();
    }
}

void BassSequence::handleLiveThreeAxis(ControlCommand command) {
    if (command.controlType != THREE_AXIS) return;
    switch (command.commandType) {
        case CHANGE_LEFT:
            _liveOctave = (uint8_t)constrain(remap(command.value, 0, 127, 0, 4), 0, 4);
            break;
        case CHANGE_RIGHT:
            _liveVelocity = command.value;
            break;
        default:
            break;
    }
}

uint8_t BassSequence::getLiveVelocity() const {
    return _liveVelocity;
}

void BassSequence::playCurrentEventNote(){
    // Mute sequence playback during recording - only monitor input
    if(isRecording()) return;
    
    pointIterator(_currentPosition);
    it->setMidiChannel(getMidiChannel());
    if(isCurrentSequenceEnabled() && it->eventState()){
        if(it->isLiteralPitch()){
            _notesPlayer.queueNote(*it);
        } else {
            _musicManager.setCurrentSteps(it->getEventRead(), BASS_SYNTH);
            it->setEventNote(_musicManager.getCurrentChordNote());
            _notesPlayer.queueNote(*it);
        }
    }
}

void BassSequence::editNoteInCurrentPosition(ControlCommand command){
  if(command.controlType == THREE_AXIS){ 
    switch(command.commandType){
      case CHANGE_LEFT:{
          pointIterator(_currentPosition);
          it->setEventRead(command.value);
          return;
      }
      case CHANGE_RIGHT:{
        pointIterator(_currentPosition);
        it->setEventVelocity(command.value);
        return;
      }
      case CHANGE_CENTER:{
          pointIterator(_currentPosition);
          it->setLength(constrain(remap(command.value, 0, 127, 1, 32),0,16));
          return;
      }
    } 
  } 
  if(command.controlType == MIDI_NOTE){
    pointIterator(_currentPosition);
    it->setEventState(true);
    it->setEventNote(command.commandType);
    it->setEventVelocity(command.value);
    return;
  }
}