#include "ChordionKeys.hpp"
#include "RTPChordMatrix.h"

// Use RTPLibrary chord tables: chordStep[][] and arpChordStep[][]
// chordStep[chordType][step] contains interval from root in semitones

void ChordionKeys::initSetup() {
    for (int i = 0; i < N_CHORDION_KEYS; i++)
        _modifierKeys[i] = false;
    _activeCount = 0;
    _ringingCount = 0;
    for (int i = 0; i < MAX_ACTIVE_CHORDS; i++)
        _activeChords[i].active = false;
}

void ChordionKeys::switchChordionKeys(int chordionIndex) {
    if (chordionIndex >= 0 && chordionIndex < N_CHORDION_KEYS)
        _modifierKeys[chordionIndex] = !_modifierKeys[chordionIndex];
}

void ChordionKeys::enableChordionKey(int chordionIndex) {
    if (chordionIndex >= 0 && chordionIndex < N_CHORDION_KEYS)
        _modifierKeys[chordionIndex] = true;
}

void ChordionKeys::disableChordionKey(int chordionIndex) {
    if (chordionIndex >= 0 && chordionIndex < N_CHORDION_KEYS)
        _modifierKeys[chordionIndex] = false;
}

uint8_t ChordionKeys::getChordType() const {
    uint8_t ct = 0;
    for (int i = 0; i < N_CHORDION_KEYS; i++)
        ct |= (_modifierKeys[i] ? (1 << i) : 0);
    return ct;
}

// New orchestrated API

ChordAction ChordionKeys::beginChord(uint8_t rootNote, uint8_t padIndex) {
    ChordAction action;
    uint8_t chordType = getChordType();
    action.chordType = chordType;
    
    // Check if this pad already has an active chord (shouldn't happen, but be safe)
    int existingIdx = _findActiveChordIndex(rootNote, padIndex);
    if (existingIdx >= 0) {
        // Reuse existing entry, update chord type
        _activeChords[existingIdx].chordType = chordType;
    } else if (_activeCount < MAX_ACTIVE_CHORDS) {
        // Add new active chord
        int slot = _activeCount;
        _activeChords[slot].rootNote = rootNote;
        _activeChords[slot].chordType = chordType;
        _activeChords[slot].padIndex = padIndex;
        _activeChords[slot].active = true;
        _activeCount++;
    }
    
    // Compute notes to play
    _computeChordNotes(rootNote, chordType, action.notesToPlay, action.playCount);
    
    // In SWEEP mode, we need to stop any chord on a different pad
    if (_mode == AutoharpMode::SWEEP) {
        for (int i = 0; i < _activeCount; i++) {
            if (_activeChords[i].active && _activeChords[i].padIndex != padIndex) {
                // This other chord needs to be stopped
                uint8_t stopNotes[16];
                uint8_t stopCount = 0;
                _computeChordNotes(_activeChords[i].rootNote, _activeChords[i].chordType, stopNotes, stopCount);
                for (uint8_t j = 0; j < stopCount && action.stopCount < 16; j++) {
                    action.notesToStop[action.stopCount++] = stopNotes[j];
                }
                _activeChords[i].active = false; // Mark for removal
            }
        }
        // Compact the array after marking sweep victims
        int writeIdx = 0;
        for (int readIdx = 0; readIdx < _activeCount; readIdx++) {
            if (_activeChords[readIdx].active) {
                if (writeIdx != readIdx)
                    _activeChords[writeIdx] = _activeChords[readIdx];
                writeIdx++;
            }
        }
        _activeCount = writeIdx;
    }
    
    return action;
}

ChordAction ChordionKeys::endChord(uint8_t rootNote, uint8_t padIndex) {
    ChordAction action;
    
    int idx = _findActiveChordIndex(rootNote, padIndex);
    if (idx >= 0) {
        uint8_t chordType = _activeChords[idx].chordType;
        action.chordType = chordType;
        
        // Compute notes to stop
        _computeChordNotes(rootNote, chordType, action.notesToStop, action.stopCount);
        
        // Remove this chord from active list
        _removeActiveChord(idx);
    }
    
    return action;
}

uint16_t ChordionKeys::getHeldPadsMask() const {
    uint16_t mask = 0;
    for (int i = 0; i < _activeCount; i++) {
        if (_activeChords[i].active && _activeChords[i].padIndex < 16)
            mask |= (1 << _activeChords[i].padIndex);
    }
    return mask;
}

bool ChordionKeys::isPadActive(uint8_t padIndex) const {
    for (int i = 0; i < _activeCount; i++) {
        if (_activeChords[i].active && _activeChords[i].padIndex == padIndex)
            return true;
    }
    return false;
}

void ChordionKeys::releaseAllChords() {
    _activeCount = 0;
    _ringingCount = 0;
    for (int i = 0; i < MAX_ACTIVE_CHORDS; i++)
        _activeChords[i].active = false;
}

// Legacy API - delegates to new system

uint8_t ChordionKeys::playChordOn(uint8_t rootNote) {
    // Use pad 255 as "legacy mode" identifier
    ChordAction action = beginChord(rootNote, 255);
    return action.chordType;
}

uint8_t ChordionKeys::releaseChordOn(uint8_t rootNote) {
    // Try to find by rootNote with legacy pad index
    int idx = -1;
    for (int i = 0; i < _activeCount; i++) {
        if (_activeChords[i].active && _activeChords[i].padIndex == 255 && _activeChords[i].rootNote == rootNote) {
            idx = i;
            break;
        }
    }
    
    if (idx >= 0) {
        uint8_t ct = _activeChords[idx].chordType;
        _removeActiveChord(idx);
        return ct;
    }
    return 0;
}

// Private helpers

int ChordionKeys::_findActiveChordIndex(uint8_t rootNote, uint8_t padIndex) const {
    for (int i = 0; i < _activeCount; i++) {
        if (_activeChords[i].active && 
            _activeChords[i].rootNote == rootNote && 
            _activeChords[i].padIndex == padIndex)
            return i;
    }
    return -1;
}

int ChordionKeys::_findRingingChordIndex(uint8_t rootNote) const {
    for (int i = 0; i < _ringingCount; i++) {
        if (_ringingChords[i].rootNote == rootNote)
            return i;
    }
    return -1;
}

void ChordionKeys::_removeActiveChord(int index) {
    if (index < 0 || index >= _activeCount) return;
    
    for (int i = index; i < _activeCount - 1; i++)
        _activeChords[i] = _activeChords[i + 1];
    
    _activeCount--;
    if (_activeCount < MAX_ACTIVE_CHORDS)
        _activeChords[_activeCount].active = false;
}

void ChordionKeys::_computeChordNotes(uint8_t rootNote, uint8_t chordType, uint8_t* notes, uint8_t& count) const {
    count = 0;
    
    // Map chordType to RTPLibrary chord index (0-15)
    // chordType is a 4-bit mask from modifier keys, directly maps to chord index
    uint8_t chordIdx = chordType & 0x0F;
    if (chordIdx >= N_CHORDS) chordIdx = 0;
    
    // Use RTPLibrary chordStep table (includes octave extensions)
    // Iterate through all steps for this chord type
    for (uint8_t i = 0; i < N_STEPS && count < 16; i++) {
        int interval = chordStep[chordIdx][i];
        // 0 in the table often indicates unused step or root duplicate
        // Include all non-zero intervals to get full voicing
        if (interval != 0 || i == 0) {  // i==0 ensures root is always included
            int note = (int)rootNote + interval;
            if (note >= 0 && note <= 127)
                notes[count++] = (uint8_t)note;
        }
    }
}