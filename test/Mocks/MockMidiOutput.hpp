#pragma once

#include <Arduino.h>
#include <memory>
#include "Interfaces/IMidiOutput.hpp"

/**
 * @brief Mock MIDI output for unit testing
 * 
 * Records all sent messages for assertion in tests.
 */
class MockMidiOutput : public IMidiOutput {
public:
    struct MidiEvent {
        enum class Type { NoteOn, NoteOff, CC, PitchBend, Raw } type;
        uint8_t byte1, byte2, byte3;
    };

    static constexpr uint8_t MAX_EVENTS = 64;

    MidiEvent _events[MAX_EVENTS];
    uint8_t _eventCount = 0;
    bool _initialized = false;

    bool initialize() override {
        _initialized = true;
        return true;
    }

    void sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) override {
        if (_eventCount < MAX_EVENTS) {
            _events[_eventCount++] = {MidiEvent::Type::NoteOn, note, velocity, channel};
        }
    }

    void sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) override {
        if (_eventCount < MAX_EVENTS) {
            _events[_eventCount++] = {MidiEvent::Type::NoteOff, note, velocity, channel};
        }
    }

    void sendControlChange(uint8_t control, uint8_t value, uint8_t channel) override {
        if (_eventCount < MAX_EVENTS) {
            _events[_eventCount++] = {MidiEvent::Type::CC, control, value, channel};
        }
    }

    void sendPitchBend(int16_t value, uint8_t channel) override {
        if (_eventCount < MAX_EVENTS) {
            _events[_eventCount++] = {MidiEvent::Type::PitchBend, 
                                      static_cast<uint8_t>(value & 0xFF), 
                                      static_cast<uint8_t>((value >> 8) & 0xFF), channel};
        }
    }

    void sendRaw(uint8_t byte1, uint8_t byte2, uint8_t byte3) override {
        if (_eventCount < MAX_EVENTS) {
            _events[_eventCount++] = {MidiEvent::Type::Raw, byte1, byte2, byte3};
        }
    }

    bool isAvailable() const override { return _initialized; }

    void reset() { _eventCount = 0; }

    bool hasNoteOn(uint8_t note, uint8_t channel = 0) const {
        for (uint8_t i = 0; i < _eventCount; i++) {
            if (_events[i].type == MidiEvent::Type::NoteOn && _events[i].byte1 == note) {
                if (channel == 0 || _events[i].byte3 == channel) return true;
            }
        }
        return false;
    }

    bool hasNoteOff(uint8_t note, uint8_t channel = 0) const {
        for (uint8_t i = 0; i < _eventCount; i++) {
            if (_events[i].type == MidiEvent::Type::NoteOff && _events[i].byte1 == note) {
                if (channel == 0 || _events[i].byte3 == channel) return true;
            }
        }
        return false;
    }

    uint8_t noteOnCount() const {
        uint8_t count = 0;
        for (uint8_t i = 0; i < _eventCount; i++) {
            if (_events[i].type == MidiEvent::Type::NoteOn) count++;
        }
        return count;
    }

    uint8_t eventCount() const { return _eventCount; }
};
