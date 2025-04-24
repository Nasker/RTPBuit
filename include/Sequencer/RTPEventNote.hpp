#pragma once

#include <cstdint>

class RTPEventNote{
    bool _state;
    uint8_t _note;
    uint8_t _read;
    uint8_t _velocity;

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
};