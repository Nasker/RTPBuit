#pragma once

#include "Arduino.h"

#define N_CHORDION_KEYS 4

struct PlayedChord {
    uint8_t rootNote = 0;
    uint8_t chordType = 0;
};

class ChordionKeys {
    bool chordionArray[N_CHORDION_KEYS] = {false, false, false, false};
    PlayedChord _ringingChords[16];   // small fixed-size ring buffer
    uint8_t     _ringingCount = 0;
public:
    ChordionKeys() = default;
    void initSetup();
    void switchChordionKeys(int chordionIndex);
    void enableChordionKey(int chordionIndex);
    void disableChordionKey(int chordionIndex);
    uint8_t getChordType() const;

    // Store a new ringing chord, return its chordType
    uint8_t playChordOn(uint8_t rootNote);
    // Remove a ringing chord for rootNote, return the stored chordType (0 if not found)
    uint8_t releaseChordOn(uint8_t rootNote);
    void releaseAllChords();
private:
    static int _findChordIndex(const PlayedChord chords[], uint8_t count, uint8_t rootNote);
};