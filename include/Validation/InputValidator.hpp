#pragma once

#include <Arduino.h>
#include "Config/MusicConfig.hpp"
#include "Validation/RangeChecker.hpp"

/**
 * @brief General input validation utilities
 * 
 * Validates user inputs for transport, sequencer, and UI operations.
 */
namespace InputValidator {

    constexpr bool isValidBPM(float bpm) {
        return bpm >= static_cast<float>(MusicConfig::Timing::MIN_BPM) &&
               bpm <= static_cast<float>(MusicConfig::Timing::MAX_BPM);
    }

    constexpr bool isValidSwing(int swing) {
        return RangeChecker::inRange<int>(swing, 
            static_cast<int>(MusicConfig::Recording::MIN_SWING),
            static_cast<int>(MusicConfig::Recording::MAX_SWING));
    }

    constexpr bool isValidQuantizeStrength(int strength) {
        return RangeChecker::inRange<int>(strength,
            static_cast<int>(MusicConfig::Recording::MIN_QUANTIZE_STRENGTH),
            static_cast<int>(MusicConfig::Recording::MAX_QUANTIZE_STRENGTH));
    }

    constexpr bool isValidSequenceLength(uint16_t length) {
        return RangeChecker::inRange<uint16_t>(length, 1, MusicConfig::Sequences::MAX_SEQUENCE_LENGTH);
    }

    constexpr bool isValidSceneIndex(uint8_t index) {
        return index < MusicConfig::Sequences::N_SCENES;
    }

    constexpr bool isValidPageIndex(uint8_t index) {
        return index < MusicConfig::Sequences::N_PAGES;
    }

    constexpr bool isValidSensorReading(int reading) {
        return RangeChecker::inRange<int>(reading, 0,
            static_cast<int>(MusicConfig::ThreeAxis::SENSOR_READING_RANGE));
    }

    constexpr bool isValidChordType(uint8_t type) {
        return type < MusicConfig::Chords::MAX_CHORD_TYPES;
    }

    constexpr bool isValidButtonIndex(uint8_t index, uint8_t maxButtons) {
        return index < maxButtons;
    }

}
