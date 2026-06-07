#include "Sequencer/RTPEventNotePlus.h"

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
    uint8_t channel = getMidiChannel();
    uint8_t note = getEventNote();
    uint8_t velocity = getEventVelocity();
    
    // USB MIDI
    usbMIDI.sendNoteOn(note, velocity, channel);
    
    // Hardware Serial1 MIDI - send raw bytes: 0x90 | (channel-1), note, velocity
    Serial1.write(0x90 | ((channel - 1) & 0x0F));
    Serial1.write(note & 0x7F);
    Serial1.write(velocity & 0x7F);
}

void RTPEventNotePlus::playNoteOff(){
    uint8_t channel = getMidiChannel();
    uint8_t note = getEventNote();
    
    // USB MIDI
    usbMIDI.sendNoteOff(note, 0, channel);
    
    // Hardware Serial1 MIDI - send raw bytes: 0x80 | (channel-1), note, velocity(0)
    Serial1.write(0x80 | ((channel - 1) & 0x0F));
    Serial1.write(note & 0x7F);
    Serial1.write(0x00);
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

bool RTPEventNotePlus::isLiteralPitch() const {
    return _literalPitch;
}

void RTPEventNotePlus::setLiteralPitch(bool literal) {
    _literalPitch = literal;
}

bool RTPEventNotePlus::decreaseTimeToLive(){
    _timeToLive--;
    if(_timeToLive <= 0){
    playNoteOff();
    return false;
    }
    return true;
}