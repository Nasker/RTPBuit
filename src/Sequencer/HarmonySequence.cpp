#include "HarmonySequence.hpp"
#include "constants.h"

HarmonySequence::HarmonySequence(int midiChannel, int NEvents, int type, int baseNote, NotesPlayer& notesPlayer, MusicManager& musicManager):
  RTPEventNoteSequence(midiChannel, NEvents, type, baseNote, notesPlayer, musicManager){
  setTypeSpecificColor();
}

void HarmonySequence::fordwardSequence(){
    _countTracker++;
    if (_countTracker >= N_COUNTS) {
        _currentPosition++;
        if (_currentPosition >= getSequenceSize())
            _currentPosition = 0;
        _countTracker = 0;
    }
}

void HarmonySequence::setTypeSpecificColor(){
    setColor(HARMONY_TRACK_COLOR_IDX);
}

void HarmonySequence::playCurrentEventNote(){
    pointIterator(_currentPosition);
    it->setMidiChannel(getMidiChannel());
    it->setDestPort(getPortAsMidiPort());
    it->setUsbHostIndex(getUsbHostDeviceIndex());
    if(isCurrentSequenceEnabled() && it->eventState()){
        int rootNote = it->getEventRead();
        int chord  = it->getEventVelocity();
        _musicManager.setCurrentHarmony(1, rootNote, chord);
        routeLiveCC(rootNote, chord, 13);
    }
}

void HarmonySequence::editNoteInCurrentPosition(ControlCommand command){
  if(command.controlType == THREE_AXIS){
    pointIterator(_currentPosition);
    switch(command.commandType){
      case CHANGE_LEFT:
          it->setEventRead(command.value);
          return;
      case CHANGE_RIGHT:
          it->setEventVelocity(command.value);
          return;
      case CHANGE_CENTER:
          it->setLength(constrain(command.value / 8, 1, 16));
          return;
    }
  }
}

void HarmonySequence::playLiveNoteOn(uint8_t rootNote, uint8_t velocity, uint8_t chordType){
    if (rootNote < BASE_NOTE) return;
    uint8_t root = (rootNote - BASE_NOTE) % N_NOTES;
    _musicManager.setCurrentHarmony(1, root, chordType & 0x0F);
}