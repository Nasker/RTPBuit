#pragma once

#include <cstdint>
#include "Sequencer/RTPEventNote.hpp"
#include "Midi/MidiPort.hpp"

class MidiRouter;


class RTPEventNotePlus: public RTPEventNote{
public:
    RTPEventNotePlus():RTPEventNote(false, 0, 0){
        setMidiChannel(0);
        setLength(1);
        setTimeToLive(1);
        setLiteralPitch(false);
        setDestPort(MidiPort::NONE);
        setUsbHostIndex(0xFF);
    }
    RTPEventNotePlus(uint8_t midiChannel, bool state, uint8_t note, uint8_t velocity):RTPEventNote(state, note, velocity){
        setMidiChannel(midiChannel);
        setLength(1);
        setTimeToLive(1);
        setLiteralPitch(false);
        setDestPort(MidiPort::NONE);
        setUsbHostIndex(0xFF);
    }
    void playNoteOn();
    void playNoteOff();
    bool decreaseTimeToLive();

    // Static router injection — set once at startup
    static void setRouter(MidiRouter* router);
private:
    static MidiRouter* _router;
};