#include "MonoSequence.hpp"
#include "ReMap.hpp"

MonoSequence::MonoSequence(int midiChannel, int NEvents, int type, int baseNote, NotesPlayer& notesPlayer, MusicManager& musicManager):
  RTPEventNoteSequence(midiChannel, NEvents, type, baseNote, notesPlayer, musicManager){
  setTypeSpecificColor();
}

void MonoSequence::setTypeSpecificColor(){
    setColor(MONO_SYNTH_COLOR);
}

void MonoSequence::playCurrentEventNote(){
    _musicManager.setCurrentSteps(it->getEventRead(), MONO_SYNTH);
    it->setEventNote(_musicManager.getCurrentChordNote());
    _notesPlayer.queueNote(*it);
}

void MonoSequence::editNoteInCurrentPosition(ControlCommand command){
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