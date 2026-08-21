#include "Sequencer/RTPEventNotePlus.h"
#include "Midi/MidiRouter.hpp"
#include "Midi/MidiMessage.hpp"

MidiRouter* RTPEventNotePlus::_router = nullptr;

void RTPEventNotePlus::setRouter(MidiRouter* router) {
    _router = router;
}

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
    
    if (_router) {
        MidiMessage msg { MidiMessage::NoteOn, channel, note, velocity, MidiPort::INTERNAL };
        _router->route(msg);
    }
}

void RTPEventNotePlus::playNoteOff(){
    uint8_t channel = getMidiChannel();
    uint8_t note = getEventNote();
    
    if (_router) {
        MidiMessage msg { MidiMessage::NoteOff, channel, note, 0, MidiPort::INTERNAL };
        _router->route(msg);
    }
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