#include "RTPEventNotePlus.h"
//#include "MIDI.h"

//MIDI_CREATE_DEFAULT_INSTANCE();

int RTPEventNotePlus::getMidiChannel(){
    return _midiChannel;
}

void RTPEventNotePlus::setMidiChannel(int midiChannel){
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

void RTPEventNotePlus::setLength(int length){
    _length = length;
    _timeToLive = _length;
}

int RTPEventNotePlus::getLength(){
    return _length;
}

int RTPEventNotePlus::getTimeToLive(){
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