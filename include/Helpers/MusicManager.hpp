#pragma once

#include "RTPMusicController.h"
#include "RTPSmartRange.h"
#include <queue>
#include "constants.h"

using namespace std;

#define BASS_OCTAVES 3
#define POLY_OCTAVES 4
#define SYNTH_OCTAVES 5

#define N_MIDI_NOTES 127


class MusicManager{
    RTPMusicController mControl;
    RTPSmartRange bassRange{BASS_OCTAVES, 7, N_MIDI_NOTES};
    RTPSmartRange synthRange{SYNTH_OCTAVES, 7, N_MIDI_NOTES};
    RTPSmartRange polyRange{POLY_OCTAVES, 7, N_MIDI_NOTES};
public:
    MusicManager();
    void setCurrentHarmony(byte channel, byte control, byte value);
    void setCurrentSteps(int rangeReading, int type);
    int getCurrentChordNote();
    queue<int> getCurrentChordNotes();
    void printCurrentHarmony();
};
