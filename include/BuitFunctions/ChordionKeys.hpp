#pragma once

#include "Arduino.h"

#define N_CHORDION_KEYS 4
#define MAX_ACTIVE_CHORDS 16

enum class AutoharpMode { SWEEP, POLYPHONIC };

// Legacy struct for backward compatibility
struct PlayedChord {
    uint8_t rootNote = 0;
    uint8_t chordType = 0;
};

// Internal tracking of a held chord
struct ActiveChord {
    uint8_t rootNote = 0;
    uint8_t chordType = 0;
    uint8_t padIndex = 0;      // 0-15, physical trellis pad
    bool    active = false;
};

// Result of a chord operation - notes to play/stop
struct ChordAction {
    uint8_t notesToPlay[16];
    uint8_t notesToStop[16];
    uint8_t playCount = 0;
    uint8_t stopCount = 0;
    uint8_t chordType = 0;
};

class ChordionKeys {
    // Modifier key state (pads 12-15)
    bool _modifierKeys[N_CHORDION_KEYS] = {false, false, false, false};
    
    // Active chord tracking for live play (pads 0-11)
    ActiveChord _activeChords[MAX_ACTIVE_CHORDS];
    uint8_t _activeCount = 0;
    AutoharpMode _mode = AutoharpMode::SWEEP;
    
    // Legacy ring buffer for backward compatibility during transition
    PlayedChord _ringingChords[16];
    uint8_t _ringingCount = 0;
    
public:
    ChordionKeys() = default;
    void initSetup();
    void setMode(AutoharpMode mode) { _mode = mode; }
    AutoharpMode getMode() const { return _mode; }
    
    // Modifier key control (pads 12-15)
    void switchChordionKeys(int chordionIndex);
    void enableChordionKey(int chordionIndex);
    void disableChordionKey(int chordionIndex);
    uint8_t getChordType() const;
    
    // New orchestrated API for live play
    // Returns action with notes to play/stop based on current mode
    ChordAction beginChord(uint8_t rootNote, uint8_t padIndex);
    ChordAction endChord(uint8_t rootNote, uint8_t padIndex);
    
    // Get bitmask of currently held pads (for LED sync)
    uint16_t getHeldPadsMask() const;
    
    // Check if a specific pad is currently holding a chord
    bool isPadActive(uint8_t padIndex) const;
    
    // Release all active chords (for state cleanup)
    void releaseAllChords();
    
    // Legacy API - will be deprecated
    uint8_t playChordOn(uint8_t rootNote);
    uint8_t releaseChordOn(uint8_t rootNote);
    
private:
    int _findActiveChordIndex(uint8_t rootNote, uint8_t padIndex) const;
    int _findRingingChordIndex(uint8_t rootNote) const;
    void _removeActiveChord(int index);
    void _computeChordNotes(uint8_t rootNote, uint8_t chordType, uint8_t* notes, uint8_t& count) const;
};