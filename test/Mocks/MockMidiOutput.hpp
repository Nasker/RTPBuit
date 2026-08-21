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
        enum class Type { NoteOn, NoteOff, CC, ProgramChange, PitchBend, RealTime, Raw } type;
        uint8_t byte1, byte2, byte3;
    };

    static constexpr uint8_t MAX_EVENTS = 64;

    MidiEvent _events[MAX_EVENTS];
    uint8_t _eventCount = 0;

    void sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) override {
        if (_eventCount < MAX_EVENTS)
            _events[_eventCount++] = {MidiEvent::Type::NoteOn, note, velocity, channel};
    }

    void sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) override {
        if (_eventCount < MAX_EVENTS)
            _events[_eventCount++] = {MidiEvent::Type::NoteOff, note, velocity, channel};
    }

    void sendControlChange(uint8_t controller, uint8_t value, uint8_t channel) override {
        if (_eventCount < MAX_EVENTS)
            _events[_eventCount++] = {MidiEvent::Type::CC, controller, value, channel};
    }

    void sendProgramChange(uint8_t program, uint8_t channel) override {
        if (_eventCount < MAX_EVENTS)
            _events[_eventCount++] = {MidiEvent::Type::ProgramChange, program, 0, channel};
    }

    void sendPitchBend(uint16_t bend, uint8_t channel) override {
        if (_eventCount < MAX_EVENTS)
            _events[_eventCount++] = {MidiEvent::Type::PitchBend,
                                      static_cast<uint8_t>(bend & 0xFF),
                                      static_cast<uint8_t>((bend >> 8) & 0xFF), channel};
    }

    void sendRealTime(uint8_t realtimebyte) override {
        if (_eventCount < MAX_EVENTS)
            _events[_eventCount++] = {MidiEvent::Type::RealTime, realtimebyte, 0, 0};
    }

    void sendRaw(const uint8_t* data, size_t length) override {
        if (length >= 3 && _eventCount < MAX_EVENTS)
            _events[_eventCount++] = {MidiEvent::Type::Raw, data[0], data[1], data[2]};
    }

    bool isReady() const override { return true; }
    void flush() override {}

    // --- Test helpers ---

    void reset() { _eventCount = 0; }

    uint8_t eventCount() const { return _eventCount; }

    bool hasNoteOn(uint8_t note, uint8_t channel = 0) const {
        for (uint8_t i = 0; i < _eventCount; i++) {
            if (_events[i].type == MidiEvent::Type::NoteOn && _events[i].byte1 == note)
                if (channel == 0 || _events[i].byte3 == channel) return true;
        }
        return false;
    }

    bool hasNoteOff(uint8_t note, uint8_t channel = 0) const {
        for (uint8_t i = 0; i < _eventCount; i++) {
            if (_events[i].type == MidiEvent::Type::NoteOff && _events[i].byte1 == note)
                if (channel == 0 || _events[i].byte3 == channel) return true;
        }
        return false;
    }

    uint8_t noteOnCount() const {
        uint8_t count = 0;
        for (uint8_t i = 0; i < _eventCount; i++)
            if (_events[i].type == MidiEvent::Type::NoteOn) count++;
        return count;
    }

    uint8_t noteOffCount() const {
        uint8_t count = 0;
        for (uint8_t i = 0; i < _eventCount; i++)
            if (_events[i].type == MidiEvent::Type::NoteOff) count++;
        return count;
    }

    // Returns true if a NoteOff for `offNote` appears before a NoteOn for `onNote`
    bool noteOffBeforeNoteOn(uint8_t offNote, uint8_t onNote) const {
        int offIdx = -1, onIdx = -1;
        for (uint8_t i = 0; i < _eventCount; i++) {
            if (offIdx == -1 && _events[i].type == MidiEvent::Type::NoteOff && _events[i].byte1 == offNote)
                offIdx = i;
            if (onIdx == -1 && _events[i].type == MidiEvent::Type::NoteOn && _events[i].byte1 == onNote)
                onIdx = i;
        }
        return (offIdx >= 0 && onIdx >= 0 && offIdx < onIdx);
    }

    // Returns true if a NoteOn for `onNote` appears before a NoteOff for `offNote` (legato)
    bool noteOnBeforeNoteOff(uint8_t onNote, uint8_t offNote) const {
        int onIdx = -1, offIdx = -1;
        for (uint8_t i = 0; i < _eventCount; i++) {
            if (onIdx == -1 && _events[i].type == MidiEvent::Type::NoteOn && _events[i].byte1 == onNote)
                onIdx = i;
            if (offIdx == -1 && _events[i].type == MidiEvent::Type::NoteOff && _events[i].byte1 == offNote)
                offIdx = i;
        }
        return (onIdx >= 0 && offIdx >= 0 && onIdx < offIdx);
    }
};
