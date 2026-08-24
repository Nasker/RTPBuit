#include "Sequencer/RTPEventNotePlus.h"
#include "Midi/MidiRouter.hpp"
#include "Midi/MidiMessage.hpp"

MidiRouter* RTPEventNotePlus::_router = nullptr;

void RTPEventNotePlus::setRouter(MidiRouter* router) {
    _router = router;
}

void RTPEventNotePlus::playNoteOn(){
    uint8_t channel = getMidiChannel();
    uint8_t note = getEventNote();
    uint8_t velocity = getEventVelocity();

    if (_router) {
        MidiMessage msg { MidiMessage::NoteOn, channel, note, velocity, MidiPort::INTERNAL };
        msg.destOverride = getDestPort();
        msg.usbHostIndex = getUsbHostIndex();
        _router->route(msg);
    }
}

void RTPEventNotePlus::playNoteOff(){
    uint8_t channel = getMidiChannel();
    uint8_t note = getEventNote();

    if (_router) {
        MidiMessage msg { MidiMessage::NoteOff, channel, note, 0, MidiPort::INTERNAL };
        msg.destOverride = getDestPort();
        msg.usbHostIndex = getUsbHostIndex();
        _router->route(msg);
    }
}

bool RTPEventNotePlus::decreaseTimeToLive(){
    uint8_t ttl = getTimeToLive();
    ttl--;
    setTimeToLive(ttl);
    if(getTimeToLive() == 0){
        playNoteOff();
        return false;
    }
    return true;
}