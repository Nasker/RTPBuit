#include "PolySequence.hpp"
#include "ReMap.hpp"

PolySequence::PolySequence(int midiChannel, int NEvents, int type, int baseNote, NotesPlayer& notesPlayer, MusicManager& musicManager):
  RTPEventNoteSequence(midiChannel, NEvents, type, baseNote, notesPlayer, musicManager){
  setTypeSpecificColor();
}

void PolySequence::setTypeSpecificColor(){
    setColor(POLY_SYNTH_COLOR_IDX);
}

void PolySequence::playLiveNoteOn(uint8_t rootNote, uint8_t velocity, uint8_t chordType) {
    uint8_t ch = getMidiChannel();
    _musicManager.setChordType(chordType);
    uint8_t steps = min(_musicManager.getChordSteps(), _liveSpread);
    uint8_t transposed = (uint8_t)((int)rootNote + _liveTranspose);
    for (uint8_t i = 0; i < steps; i++) {
        uint8_t note = transposed + _musicManager.getChordStep(i);
        usbMIDI.sendNoteOn(note, _liveVelocity, ch);
        Serial1.write(0x90 | ((ch - 1) & 0x0F));
        Serial1.write(note & 0x7F);
        Serial1.write(_liveVelocity & 0x7F);
    }
}

void PolySequence::playLiveNoteOff(uint8_t rootNote, uint8_t chordType) {
    uint8_t ch = getMidiChannel();
    _musicManager.setChordType(chordType);
    uint8_t steps = min(_musicManager.getChordSteps(), _liveSpread);
    uint8_t transposed = (uint8_t)((int)rootNote + _liveTranspose);
    for (uint8_t i = 0; i < steps; i++) {
        uint8_t note = transposed + _musicManager.getChordStep(i);
        usbMIDI.sendNoteOff(note, 0, ch);
        Serial1.write(0x80 | ((ch - 1) & 0x0F));
        Serial1.write(note & 0x7F);
        Serial1.write(0x00);
    }
}

void PolySequence::handleLiveThreeAxis(ControlCommand command) {
    if (command.controlType != THREE_AXIS) return;
    switch (command.commandType) {
        case CHANGE_LEFT:
            _liveTranspose = (int8_t)constrain(remap(command.value, 0, 127, -12, 12), -12, 12);
            break;
        case CHANGE_CENTER:
            _liveSpread = (uint8_t)constrain(remap(command.value, 0, 127, 1, 8), 1, 8);
            break;
        case CHANGE_RIGHT:
            _liveVelocity = command.value;
            break;
    }
}

uint8_t PolySequence::getLiveVelocity() const {
    return _liveVelocity;
}

void PolySequence::playCurrentEventNote(){
    // Mute sequence playback during recording - only monitor input
    if(isRecording()) return;
    
    pointIterator(_currentPosition);
    it->setMidiChannel(getMidiChannel());
    if(isCurrentSequenceEnabled() && it->eventState()){
        if(it->isLiteralPitch()){
            _notesPlayer.queueNote(*it);
        } else {
            int spread = constrain(remap(it->getLength(), 1, 16, 1, 8), 1, 8);
            auto chordNotes = _musicManager.getAutoharpChordNotes(it->getEventRead(), spread);
            while(!chordNotes.empty()){
                it->setEventNote(chordNotes.front());
                _notesPlayer.queueNote(*it);
                chordNotes.pop();
            }
        }
    }
}

void PolySequence::editNoteInCurrentPosition(ControlCommand command){
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