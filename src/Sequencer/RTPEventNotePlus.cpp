#include "Sequencer/RTPEventNotePlus.h"
//#include "MIDI.h"

//MIDI_CREATE_DEFAULT_INSTANCE();

uint8_t RTPEventNotePlus::getMidiChannel(){
    return _midiChannel;
}

// Const version for JSON serialization
uint8_t RTPEventNotePlus::getMidiChannel() const {
    return _midiChannel;
}

void RTPEventNotePlus::setMidiChannel(uint8_t midiChannel){
    _midiChannel = midiChannel;
}

void RTPEventNotePlus::playNoteOn(){
    usbMIDI.sendNoteOn(getEventNote(), getEventVelocity(), getMidiChannel());
    //MIDI.sendNoteOn(getEventNote(), getEventVelocity(), getMidiChannel());
}

void RTPEventNotePlus::playNoteOff(){
    usbMIDI.sendNoteOff(getEventNote(), 0, getMidiChannel());
    //MIDI.sendNoteOff(getEventNote(), 0, getMidiChannel());
}

void RTPEventNotePlus::setLength(uint8_t length){
    _length = length;
    _timeToLive = _length;
}

uint8_t RTPEventNotePlus::getLength(){
    return _length;
}

// Const version for JSON serialization
uint8_t RTPEventNotePlus::getLength() const {
    return _length;
}

uint8_t RTPEventNotePlus::getTimeToLive(){
    return _timeToLive;
}

bool RTPEventNotePlus::decreaseTimeToLive(){
    _timeToLive--;
    if(_timeToLive <= 0){
    playNoteOff();
    return false;
    }
    return true;
}