#pragma once

#include "Arduino.h"
#include "RTPMusicController.h"
#include <list>

using namespace std;

#define N_CHORDION_KEYS 4
#define N_CHANNELS 4

struct PlayedChord{
    int rootNote=0;
    int chordType=0;
};

struct InstrumentChannel{
    int midiChannel;
    int voices;
};

const int voicesPerInstrument[N_CHANNELS] = {-1,0,1,2};

class ChordionKeys{
    bool chordionArray[N_CHORDION_KEYS] = {false,false,false,false};
    InstrumentChannel instChannel[N_CHANNELS];
    RTPMusicController mController;
    list<PlayedChord> ringingChordsList;
public:
    ChordionKeys();
    void initSetup();
    void switchChordionKeys(int chordionIndex);
    void enableChordionKey(int chordionIndex);
    void disableChordionKey(int chordionIndex);
    PlayedChord playChord(int rootNote);
    void releaseChord(int rootNote);
private:
    void printChordionArray();
};