#include "MusicManager.hpp"
#include <cstdint>

MusicManager::MusicManager(){}

void MusicManager::setCurrentHarmony(uint8_t channel, uint8_t control, uint8_t value){
    if(channel == 1 && control>=0 && control<=15 && value<=15){ 
        mControl.setCurrentRootNote(control);
        mControl.setCurrentScale(value);
        mControl.setCurrentChord(value);
        bassRange.setNumberStepsInZone(mControl.chords.getChordSteps());
        synthRange.setNumberStepsInZone(mControl.chords.getChordSteps());
        polyRange.setNumberStepsInZone(mControl.chords.getChordSteps());
    }
}

void MusicManager::printCurrentHarmony(){
    Serial.print(mControl.getCurrentRootNoteName());
    Serial.print("\t");
    Serial.println(mControl.getChordName());
}

void MusicManager::setCurrentSteps(int rangeReading, int type){
    switch(type){
        case BASS_SYNTH:{
            mControl.setCurrentChordStep(bassRange.getCurrentStepInZone(rangeReading));
            mControl.setCurrentOctave(bassRange.getCurrentZone(rangeReading));
            return;
        }
        case MONO_SYNTH:{
            mControl.setCurrentChordStep(synthRange.getCurrentStepInZone(rangeReading));
            mControl.setCurrentOctave(synthRange.getCurrentZone(rangeReading));
            return;
        }
        case POLY_SYNTH:{
            mControl.setCurrentChordStep(polyRange.getCurrentStepInZone(rangeReading));
            mControl.setCurrentOctave(polyRange.getCurrentZone(rangeReading));
            return;
        }
    }
}

int MusicManager::getCurrentChordNote(){
    return mControl.getCurrentArpChordMidiNote();
}

queue<int> MusicManager::getCurrentChordNotes(){
    queue<int> chordNotes;
    for(int i=0; i<mControl.chords.getChordSteps(); i++){
        mControl.setCurrentChordStep(i);
        chordNotes.push(mControl.getCurrentChordMidiNote());
    }
    return chordNotes;
}

queue<int> MusicManager::getAutoharpChordNotes(int rangeReading, int spread){
    queue<int> chordNotes;
    uint8_t chordSteps = mControl.chords.getChordSteps();
    uint8_t poolSize = POLY_OCTAVES * chordSteps;
    uint8_t focus = constrain((int)polyRange.getCurrentStep(rangeReading), 0, poolSize - 1);
    uint8_t octave = focus / chordSteps;
    uint8_t count = max(1, (uint8_t)round(float(spread) * float(octave + 1) / float(POLY_OCTAVES)));
    count = min(count, poolSize - focus);
    for(uint8_t i = focus; i < focus + count; i++){
        mControl.setCurrentOctave(i / chordSteps);
        mControl.setCurrentChordStep(i % chordSteps);
        chordNotes.push(mControl.getCurrentChordMidiNote());
    }
    return chordNotes;
}