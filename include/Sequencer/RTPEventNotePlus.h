#pragma once

#include "Sequencer/RTPEventNote.hpp"
#include "Arduino.h"


class RTPEventNotePlus: public RTPEventNote{
    int _midiChannel;
    int _length;
    int _timeToLive;
  public:
    RTPEventNotePlus(int midiChannel, bool state, int note, int velocity):RTPEventNote(state, note, velocity){
        _midiChannel = midiChannel;
        _length = 1;
        _timeToLive = _length;
  }
    int getMidiChannel();
    int getMidiChannel() const;  // Const version for JSON serialization
    void setMidiChannel(int midiChannel);
    void playNoteOn();
    void playNoteOff();
    void setLength(int length);
    int getLength();
    int getLength() const;  // Const version for JSON serialization
    int getTimeToLive();
    bool decreaseTimeToLive();
};