#include "constants.h"

const char* NOTE_NAMES[12] = {
    "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
};

// Names aligned with RTPLibrary chordStep[N_CHORDS] interval table:
// 0 mono, 1 maj, 2 min, 3 maj7, 4 min7, 5 dom7, 6 dim, 7 dim7,
// 8 m7b5, 9 aug, 10 maj9, 11 min9, 12 dom9, 13 sus4, 14 sus2, 15 6th
const char* CHORD_TYPE_NAMES[16] = {
    "note", "maj", "min", "maj7", "min7", "dom7", "dim", "dim7",
    "m7b5", "aug", "maj9", "min9", "dom9", "sus4", "sus2", "6th"
};
