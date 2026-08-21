#pragma once

#include <Arduino.h>
#include "Config/MusicConfig.hpp"
#include "Validation/RangeChecker.hpp"

/**
 * @brief MIDI message validation utilities
 * 
 * Validates MIDI values against the spec and configured ranges.
 */
namespace MidiValidator {

    constexpr bool isValidNote(uint8_t note) {
        return RangeChecker::inRange<uint8_t>(note, MusicConfig::Notes::MIN_MIDI_NOTE,
                                              MusicConfig::Notes::MAX_MIDI_NOTE);
    }

    constexpr bool isValidVelocity(uint8_t velocity) {
        return RangeChecker::inRange<uint8_t>(velocity, MusicConfig::Velocity::MIN_VELOCITY,
                                              MusicConfig::Velocity::MAX_VELOCITY);
    }

    constexpr bool isValidChannel(uint8_t channel) {
        return RangeChecker::inRange<uint8_t>(channel, 1, 16);
    }

    constexpr bool isValidControlValue(uint8_t value) {
        return RangeChecker::inRange<uint8_t>(value, 0, 127);
    }

    constexpr bool isValidControlNumber(uint8_t cc) {
        return RangeChecker::inRange<uint8_t>(cc, 0, 127);
    }

    constexpr bool isValidPitchBend(int16_t value) {
        return RangeChecker::inRange<int16_t>(value, -8192, 8191);
    }

    constexpr bool isNoteOff(uint8_t velocity) {
        return velocity == 0;
    }

}
