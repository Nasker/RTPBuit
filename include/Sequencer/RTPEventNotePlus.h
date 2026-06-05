#pragma once

#include <cstdint>
#include "Sequencer/RTPEventNote.hpp"
#include "Arduino.h"


class RTPEventNotePlus: public RTPEventNote{
    uint8_t _midiChannel;
    uint8_t _length;
    uint8_t _timeToLive;
    bool _literalPitch;
  public:
    RTPEventNotePlus():RTPEventNote(false, 0, 0){
        _midiChannel = 0;
        _length = 1;
        _timeToLive = 1;
        _literalPitch = false;
    }
    RTPEventNotePlus(uint8_t midiChannel, bool state, uint8_t note, uint8_t velocity):RTPEventNote(state, note, velocity){
        _midiChannel = midiChannel;
        _length = 1;
        _timeToLive = _length;
        _literalPitch = false;
  }
    uint8_t getMidiChannel();
    uint8_t getMidiChannel() const;  // Const version for JSON serialization
    void setMidiChannel(uint8_t midiChannel);
    void playNoteOn();
    void playNoteOff();
    void setLength(uint8_t length);
    uint8_t getLength();
    uint8_t getLength() const;  // Const version for JSON serialization
    uint8_t getTimeToLive();
    bool decreaseTimeToLive();
    bool isLiteralPitch() const;
    void setLiteralPitch(bool literal);
};