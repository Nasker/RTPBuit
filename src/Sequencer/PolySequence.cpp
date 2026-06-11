#include "PolySequence.hpp"
#include "ReMap.hpp"

PolySequence::PolySequence(int midiChannel, int NEvents, int type, int baseNote, NotesPlayer& notesPlayer, MusicManager& musicManager):
  RTPEventNoteSequence(midiChannel, NEvents, type, baseNote, notesPlayer, musicManager){
  setTypeSpecificColor();
}

void PolySequence::setTypeSpecificColor(){
    setColor(POLY_SYNTH_COLOR_IDX);
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