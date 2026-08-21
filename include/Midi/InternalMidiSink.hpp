#pragma once

#include "Interfaces/IMidiOutput.hpp"
#include <functional>

/**
 * @brief MIDI output sink that feeds messages into the internal engine
 * 
 * When external MIDI messages are routed to INTERNAL, this class
 * dispatches them to the appropriate internal handlers (note routing,
 * CC routing, real-time/clock handling) via callbacks.
 */
class InternalMidiSink : public IMidiOutput {
public:
    using NoteCallback = std::function<void(uint8_t channel, uint8_t note, uint8_t velocity)>;
    using CCCallback = std::function<void(uint8_t channel, uint8_t controller, uint8_t value)>;
    using RealTimeCallback = std::function<void(uint8_t realtimebyte)>;

    InternalMidiSink() = default;

    void setNoteOnCallback(NoteCallback cb)    { _onNoteOn = cb; }
    void setNoteOffCallback(NoteCallback cb)   { _onNoteOff = cb; }
    void setCCCallback(CCCallback cb)          { _onCC = cb; }
    void setRealTimeCallback(RealTimeCallback cb) { _onRealTime = cb; }

    // IMidiOutput interface
    void sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) override;
    void sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) override;
    void sendControlChange(uint8_t controller, uint8_t value, uint8_t channel) override;
    void sendProgramChange(uint8_t program, uint8_t channel) override;
    void sendPitchBend(uint16_t bend, uint8_t channel) override;
    void sendRealTime(uint8_t realtimebyte) override;
    void sendRaw(const uint8_t* data, size_t length) override;
    bool isReady() const override;
    void flush() override;

private:
    NoteCallback     _onNoteOn;
    NoteCallback     _onNoteOff;
    CCCallback       _onCC;
    RealTimeCallback _onRealTime;
};
