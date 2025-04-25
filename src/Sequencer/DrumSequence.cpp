#include "DrumSequence.hpp"

DrumSequence::DrumSequence(int midiChannel, int NEvents, int type, int baseNote, NotesPlayer& notesPlayer, MusicManager& musicManager):
  RTPEventNoteSequence(midiChannel, NEvents, type, baseNote, notesPlayer, musicManager){
  setTypeSpecificColor();
}

void DrumSequence::setTypeSpecificColor(){
    setColor(DRUM_COLOR);
}

void DrumSequence::playCurrentEventNote(){
    pointIterator(_currentPosition);
    it->setMidiChannel(getMidiChannel());
    if(isCurrentSequenceEnabled() && it->eventState()){
        it->setLength(1);
        _notesPlayer.queueNote(*it);
    }
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