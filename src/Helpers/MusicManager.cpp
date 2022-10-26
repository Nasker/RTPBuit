#include "MusicManager.hpp"

MusicManager::MusicManager(){}

void MusicManager::setCurrentHarmony(byte channel, byte control, byte value){
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