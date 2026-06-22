#pragma once

#include "BuitFunctions/ChordionKeys.hpp"
#include "Error/Result.hpp"
#include <Arduino.h>

/**
 * @brief Manager for live play orchestration
 * 
 * Handles ChordionKeys, drum rolls, and live performance state.
 * Follows Single Responsibility Principle by focusing only on live-play logic.
 */
class LivePlayManager {
private:
    ChordionKeys _chordionKeys;
    bool _ready;
    
    // Drum roll state
    bool _drumRollActive;
    uint8_t _drumRollNote;
    uint8_t _rollDivision;
    uint32_t _tickCount;
    uint32_t _fineTickCount;

public:
    LivePlayManager() 
        : _ready(false), 
          _drumRollActive(false), 
          _drumRollNote(36), 
          _rollDivision(1),
          _tickCount(0),
          _fineTickCount(0) {}

    /**
     * @brief Initialize the live play manager
     * @return Result indicating success or failure
     */
    Result<void> initialize() {
        _chordionKeys.initSetup();
        _ready = true;
        return Result<void>::Ok();
    }

    /**
     * @brief Check if manager is ready
     * @return true if ready
     */
    bool isReady() const { return _ready; }

    // ChordionKeys delegation
    ChordAction beginChord(uint8_t rootNote, uint8_t padIndex) {
        return _chordionKeys.beginChord(rootNote, padIndex);
    }

    ChordAction endChord(uint8_t rootNote, uint8_t padIndex) {
        return _chordionKeys.endChord(rootNote, padIndex);
    }

    void releaseAllChords() {
        _chordionKeys.releaseAllChords();
    }

    void enableChordionKey(uint8_t modifierIndex) {
        _chordionKeys.enableChordionKey(modifierIndex);
    }

    void disableChordionKey(uint8_t modifierIndex) {
        _chordionKeys.disableChordionKey(modifierIndex);
    }

    uint8_t getChordType() const {
        return _chordionKeys.getChordType();
    }

    uint16_t getHeldPadsMask() const {
        return _chordionKeys.getHeldPadsMask();
    }

    // Drum roll state
    void setDrumRollActive(bool active) { _drumRollActive = active; }
    bool isDrumRollActive() const { return _drumRollActive; }
    
    void setDrumRollNote(uint8_t note) { _drumRollNote = note; }
    uint8_t getDrumRollNote() const { return _drumRollNote; }
    
    void setRollDivision(uint8_t division) { _rollDivision = division; }
    uint8_t getRollDivision() const { return _rollDivision; }

    // Tick management
    void advanceTick() { _tickCount++; }
    void advanceFineTick() { _fineTickCount++; }
    
    uint32_t getTickCount() const { return _tickCount; }
    uint32_t getFineTickCount() const { return _fineTickCount; }
    
    void resetTicks() {
        _tickCount = 0;
        _fineTickCount = 0;
    }

    // Drum roll logic
    bool shouldTriggerDrumRoll() const {
        if (!_drumRollActive || _rollDivision == 0) return false;
        return (_fineTickCount % _rollDivision) == 0;
    }
};
