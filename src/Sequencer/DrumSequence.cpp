#include "DrumSequence.hpp"

DrumSequence::DrumSequence(int midiChannel, int NEvents, int type, int baseNote, NotesPlayer& notesPlayer, MusicManager& musicManager):
  RTPEventNoteSequence(midiChannel, NEvents, type, baseNote, notesPlayer, musicManager){
  setTypeSpecificColor();
}

void DrumSequence::setTypeSpecificColor(){
    setColor(DRUM_COLOR_IDX);
}

void DrumSequence::playCurrentEventNote(){
    pointIterator(_currentPosition);
    it->setMidiChannel(getMidiChannel());
    it->setDestPort(getPortAsMidiPort());
    if(isCurrentSequenceEnabled() && it->eventState()){
        it->setLength(1);
        _notesPlayer.queueNote(*it);
    }
}

void DrumSequence::playLiveNoteOn(uint8_t rootNote, uint8_t velocity, uint8_t chordType) {
    (void)velocity;
    (void)chordType;
    uint8_t ch = getMidiChannel();
    routeLiveNoteOn(rootNote, _liveVelocity, ch);
}

void DrumSequence::playLiveNoteOff(uint8_t rootNote, uint8_t chordType) {
    (void)chordType;
    uint8_t ch = getMidiChannel();
    routeLiveNoteOff(rootNote, ch);
}

void DrumSequence::handleLiveThreeAxis(ControlCommand command) {
    if (command.controlType != THREE_AXIS) return;
    if (command.commandType == CHANGE_RIGHT)
        _liveVelocity = command.value;
}

uint8_t DrumSequence::getLiveVelocity() const {
    return _liveVelocity;
}

void DrumSequence::editNoteInCurrentPosition(ControlCommand command){
    switch(command.controlType){
        case THREE_AXIS:
            if(command.commandType == CHANGE_RIGHT){
                pointIterator(_currentPosition);
                it->setEventVelocity(command.value);
                return;
            }
            return;
        case MIDI_NOTE:
            pointIterator(_currentPosition);
            it->setEventState(true);
            it->setEventVelocity(command.value);
            Serial.printf("DrumSequence::editNoteInCurrentPosition: %d\n", command.value);
            return;
    }
}