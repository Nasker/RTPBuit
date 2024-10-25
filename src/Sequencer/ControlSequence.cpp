#include "ControlSequence.hpp"

ControlSequence::ControlSequence(int midiChannel, int NEvents, int type, int baseNote, NotesPlayer& notesPlayer, MusicManager& musicManager):
  RTPEventNoteSequence(midiChannel, NEvents, type, baseNote, notesPlayer, musicManager){
  setTypeSpecificColor();
}

void ControlSequence::setTypeSpecificColor(){
    setColor(CONTROL_TRACK_COLOR);
}

void ControlSequence::playCurrentEventNote(){
    int rootNote = it->getEventRead();
    int chord  = it->getEventVelocity();
    _musicManager.setCurrentHarmony(1, rootNote, chord);
}

void ControlSequence::editNoteInCurrentPosition(ControlCommand command){
    if(command.controlType == MIDI_CC){ 
        pointIterator(_currentPosition);
        it->setEventRead(command.commandType);
        it->setEventVelocity(command.value);
    }
}