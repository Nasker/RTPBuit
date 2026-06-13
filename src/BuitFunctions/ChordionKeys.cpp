#include "ChordionKeys.hpp"

void ChordionKeys::initSetup() {
    for (int i = 0; i < N_CHORDION_KEYS; i++)
        chordionArray[i] = false;
    _ringingCount = 0;
}

void ChordionKeys::switchChordionKeys(int chordionIndex) {
    chordionArray[chordionIndex] = !chordionArray[chordionIndex];
}

void ChordionKeys::enableChordionKey(int chordionIndex) {
    chordionArray[chordionIndex] = true;
}

void ChordionKeys::disableChordionKey(int chordionIndex) {
    chordionArray[chordionIndex] = false;
}

uint8_t ChordionKeys::getChordType() const {
    uint8_t ct = 0;
    for (int i = 0; i < N_CHORDION_KEYS; i++)
        ct |= (chordionArray[i] ? (1 << i) : 0);
    return ct;
}

uint8_t ChordionKeys::playChordOn(uint8_t rootNote) {
    uint8_t ct = getChordType();
    int idx = _findChordIndex(_ringingChords, _ringingCount, rootNote);
    if (idx >= 0) {
        _ringingChords[idx].chordType = ct;
        return ct;
    }
    if (_ringingCount < 16) {
        _ringingChords[_ringingCount].rootNote = rootNote;
        _ringingChords[_ringingCount].chordType = ct;
        _ringingCount++;
    }
    return ct;
}

uint8_t ChordionKeys::releaseChordOn(uint8_t rootNote) {
    int idx = _findChordIndex(_ringingChords, _ringingCount, rootNote);
    if (idx < 0) return 0;
    uint8_t ct = _ringingChords[idx].chordType;
    // shift remaining entries down
    for (int i = idx; i < (int)_ringingCount - 1; i++)
        _ringingChords[i] = _ringingChords[i + 1];
    _ringingCount--;
    return ct;
}

void ChordionKeys::releaseAllChords() {
    _ringingCount = 0;
}

int ChordionKeys::_findChordIndex(const PlayedChord chords[], uint8_t count, uint8_t rootNote) {
    for (int i = 0; i < count; i++) {
        if (chords[i].rootNote == rootNote)
            return i;
    }
    return -1;
}