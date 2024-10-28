#include "ControlSequence.hpp"

ControlSequence::ControlSequence(int midiChannel, int NEvents, int type, int baseNote, NotesPlayer& notesPlayer, MusicManager& musicManager):
  RTPEventNoteSequence(midiChannel, NEvents, type, baseNote, notesPlayer, musicManager){
  setTypeSpecificColor();
}

void ControlSequence::setTypeSpecificColor(){
    setColor(CONTROL_TRACK_COLOR);
}

void ControlSequence::playCurrentEventNote(){
    pointIterator(_currentPosition);
    it->setMidiChannel(getMidiChannel());
    if(isCurrentSequenceEnabled() && it->eventState()){
        int read = it->getEventRead();
        int value  = it->getEventVelocity();
    }
}

void ControlSequence::editNoteInCurrentPosition(ControlCommand command){
    if(command.controlType == MIDI_CC){ 
        pointIterator(_currentPosition);
        it->setEventRead(command.commandType);
        it->setEventVelocity(command.value);
    }
}