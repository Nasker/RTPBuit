#include "PolySequence.hpp"
#include "ReMap.hpp"

PolySequence::PolySequence(int midiChannel, int NEvents, int type, int baseNote, NotesPlayer& notesPlayer, MusicManager& musicManager):
  RTPEventNoteSequence(midiChannel, NEvents, type, baseNote, notesPlayer, musicManager){
  setTypeSpecificColor();
}

void PolySequence::setTypeSpecificColor(){
    setColor(POLY_SYNTH_COLOR);
}

void PolySequence::playCurrentEventNote(){
    _musicManager.setCurrentSteps(it->getEventRead(), POLY_SYNTH);
    auto chordNotes = _musicManager.getCurrentChordNotes();
    while(!chordNotes.empty()){
      it->setEventNote(chordNotes.front());
      _notesPlayer.queueNote(*it);
      chordNotes.pop();
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
}