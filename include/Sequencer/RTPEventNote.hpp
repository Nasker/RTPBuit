#pragma once

#include <cstdint>
#include "Midi/MidiPort.hpp"

class RTPEventNote{
protected:
    uint32_t _low;
    uint32_t _high;

public:
    RTPEventNote(bool state, uint8_t note);
    RTPEventNote(bool state, uint8_t note, uint8_t velocity);
    bool eventState();
    bool eventState() const;  // Const version for JSON serialization
    uint8_t getEventNote();
    uint8_t getEventNote() const;  // Const version for JSON serialization
    uint8_t getEventRead();
    uint8_t getEventRead() const;  // Const version for JSON serialization
    uint8_t getEventVelocity();
    uint8_t getEventVelocity() const;  // Const version for JSON serialization
    void setEventState(bool state);
    void setEventNote(uint8_t note);
    void setEventRead(uint8_t read);
    void setEventVelocity(uint8_t velocity);
    void switchState();

    // Packed sequence/runtime fields (also used by RTPEventNotePlus)
    uint8_t getMidiChannel();
    uint8_t getMidiChannel() const;
    void setMidiChannel(uint8_t midiChannel);
    uint8_t getLength() const;
    void setLength(uint8_t length);
    uint8_t getTimeToLive() const;
    void setTimeToLive(uint8_t timeToLive);
    MidiPort getDestPort() const;
    void setDestPort(MidiPort port);
    uint8_t getUsbHostIndex() const;
    void setUsbHostIndex(uint8_t idx);
    bool isLiteralPitch() const;
    void setLiteralPitch(bool literal);

    // Raw packed word access for binary persistence
    uint32_t getPackedLow() const { return _low; }
    uint32_t getPackedHigh() const { return _high; }
    void setPacked(uint32_t low, uint32_t high) { _low = low; _high = high; }
};