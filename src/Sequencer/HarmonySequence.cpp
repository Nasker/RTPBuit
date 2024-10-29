#include "HarmonySequence.hpp"

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
    setColor(HARMONY_TRACK_COLOR);
}

void HarmonySequence::playCurrentEventNote(){
    pointIterator(_currentPosition);
    it->setMidiChannel(getMidiChannel());
    if(isCurrentSequenceEnabled() && it->eventState()){
        int rootNote = it->getEventRead();
        int chord  = it->getEventVelocity();
        _musicManager.setCurrentHarmony(1, rootNote, chord);
        Serial.printf(">>>>>>Setting Harmony with %d %d\n\r", rootNote,
        chord);
    }
}

void HarmonySequence::editNoteInCurrentPosition(ControlCommand command){
    if(command.controlType == MIDI_CC){
        if(command.commandType > 11)
            return;
        Serial.printf("<<<<<<Editting Harmony with %d %d\n\r", command.commandType,
        command.value);
        pointIterator(_currentPosition);
        it->setEventRead(command.commandType);
        it->setEventVelocity(command.value);
    }
}