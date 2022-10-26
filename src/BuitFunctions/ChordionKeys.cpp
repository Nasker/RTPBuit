#include "ChordionKeys.hpp"
#include "constants.h"
#include "MIDI.h"
#include "Audio.h"

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

ChordionKeys::ChordionKeys(){
    MIDI.begin(MIDI_CHANNEL_OMNI);
}

void ChordionKeys::initSetup(){
    for(int i=0; i<N_CHORDION_KEYS; i++)
        chordionArray[i] = false;
    for(int i=0; i<N_CHANNELS; i++){
        instChannel[i].midiChannel = i + 1;
        instChannel[i].voices = voicesPerInstrument[i];
    }
}

void ChordionKeys::switchChordionKeys(int chordionIndex){
    chordionArray[chordionIndex] = !chordionArray[chordionIndex];
}

void ChordionKeys::enableChordionKey(int chordionIndex){
    chordionArray[chordionIndex] = true;
}

void ChordionKeys::disableChordionKey(int chordionIndex){
    chordionArray[chordionIndex] = false;
}

PlayedChord ChordionKeys::playChord(int rootNote){
    PlayedChord playedChord;
    playedChord.rootNote = rootNote;
    playedChord.chordType = 0;
    for(int i=0;i<N_CHORDION_KEYS;i++){
        playedChord.chordType += int(chordionArray[i])*pow(2,i);
    }
    mController.chords.setChordType(playedChord.chordType);
    ringingChordsList.push_back(playedChord);
    usbMIDI.sendControlChange(rootNote - BASE_NOTE - N_NOTES, playedChord.chordType, 1);
    //usbMIDI.sendProgramChange(playedChord.chordType,2);
    for(int j=0; j<N_CHANNELS; j++){
        for(int i=0; i< mController.chords.getChordSteps();i++){
            if(instChannel[j].voices == -1 && mController.chords.getChordStep(i) < 0){
                usbMIDI.sendNoteOn(rootNote+mController.chords.getChordStep(i), 90, instChannel[j].midiChannel);
                MIDI.sendNoteOn(rootNote+mController.chords.getChordStep(i), 90, instChannel[j].midiChannel);
                //Serial.printf("CHANNEL %d\n", instChannel[j].midiChannel);
            }
            else if(instChannel[j].voices == 0){
                usbMIDI.sendNoteOn(rootNote+mController.chords.getChordStep(i), 90, instChannel[j].midiChannel);
                MIDI.sendNoteOn(rootNote+mController.chords.getChordStep(i), 90, instChannel[j].midiChannel);
                //Serial.printf("CHANNEL %d\n", instChannel[j].midiChannel);
            }
            else if(instChannel[j].voices == 1 && mController.chords.getChordStep(i) >= 0 && mController.chords.getChordStep(i) < 12){
                usbMIDI.sendNoteOn(rootNote+mController.chords.getChordStep(i), 90, instChannel[j].midiChannel);
                MIDI.sendNoteOn(rootNote+mController.chords.getChordStep(i), 90, instChannel[j].midiChannel);
                //Serial.printf("CHANNEL %d\n", instChannel[j].midiChannel);
            }
            else if(instChannel[j].voices == 2 && mController.chords.getChordStep(i) >= 12){
                usbMIDI.sendNoteOn(rootNote+mController.chords.getChordStep(i), 90, instChannel[j].midiChannel);
                MIDI.sendNoteOn(rootNote+mController.chords.getChordStep(i), 90, instChannel[j].midiChannel);
                //Serial.printf("CHANNEL %d\n", instChannel[j].midiChannel);
            }

            
        }
    }
    return playedChord;
}

void ChordionKeys::releaseChord(int rootNote){
    list<PlayedChord>::iterator it;
    for(it = ringingChordsList.begin(); it != ringingChordsList.end(); it++){
        if(it->rootNote == rootNote){
            mController.chords.setChordType(it->chordType);
            for(int k=0; k<N_CHANNELS; k++){
                for(int j=0; j< mController.chords.getChordSteps();j++){
                    if(instChannel[k].voices == -1 && mController.chords.getChordStep(j) < 0){
                        usbMIDI.sendNoteOff(rootNote+mController.chords.getChordStep(j), 90, instChannel[k].midiChannel);
                        MIDI.sendNoteOff(rootNote+mController.chords.getChordStep(j), 90, instChannel[k].midiChannel);
                    }
                    if(instChannel[k].voices == 0){
                        usbMIDI.sendNoteOff(rootNote+mController.chords.getChordStep(j), 90, instChannel[k].midiChannel);
                        MIDI.sendNoteOff(rootNote+mController.chords.getChordStep(j), 90, instChannel[k].midiChannel);
                    }
                    if(instChannel[k].voices == 1 && mController.chords.getChordStep(j) >= 0 && mController.chords.getChordStep(j) < 12){
                        usbMIDI.sendNoteOff(rootNote+mController.chords.getChordStep(j), 90, instChannel[k].midiChannel);
                        MIDI.sendNoteOff(rootNote+mController.chords.getChordStep(j), 90, instChannel[k].midiChannel);
                    }
                    if(instChannel[k].voices == 2 && mController.chords.getChordStep(j) >= 12){
                        usbMIDI.sendNoteOff(rootNote+mController.chords.getChordStep(j), 90, instChannel[k].midiChannel);
                        MIDI.sendNoteOff(rootNote+mController.chords.getChordStep(j), 90, instChannel[k].midiChannel);
                    }
                }
            }   
            it = ringingChordsList.erase(it);
        }
    }
}

void ChordionKeys::printChordionArray(){
    for(int i=0; i<N_CHORDION_KEYS; i++)
        Serial.printf("%d ", chordionArray[i]);
    
    Serial.println();
}