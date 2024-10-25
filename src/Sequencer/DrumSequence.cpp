#include "DrumSequence.hpp"

DrumSequence::DrumSequence(int midiChannel, int NEvents, int type, int baseNote, NotesPlayer& notesPlayer, MusicManager& musicManager):
  RTPEventNoteSequence(midiChannel, NEvents, type, baseNote, notesPlayer, musicManager){
  setTypeSpecificColor();
}

void DrumSequence::setTypeSpecificColor(){
    setColor(DRUM_COLOR);
}

void DrumSequence::playCurrentEventNote(){
    if (it != EventNoteSequence.end() && !EventNoteSequence.empty()) {
        it->setLength(1);
        _notesPlayer.queueNote(*it);
    } else
        Serial.println("Iterator is invalid or EventNoteSequence is empty!");
}

void DrumSequence::editNoteInCurrentPosition(ControlCommand command){
    if (command.commandType == CHANGE_RIGHT){
        pointIterator(_currentPosition);
        it->setEventRead(command.value);
    }
}