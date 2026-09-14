#include "Midi/InternalMidiSink.hpp"

void InternalMidiSink::sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) {
    if (_onNoteOn) _onNoteOn(channel, note, velocity, _srcPort, _srcDevice);
}

void InternalMidiSink::sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) {
    if (_onNoteOff) _onNoteOff(channel, note, velocity, _srcPort, _srcDevice);
}

void InternalMidiSink::sendControlChange(uint8_t controller, uint8_t value, uint8_t channel) {
    if (_onCC) _onCC(channel, controller, value, _srcPort, _srcDevice);
}

void InternalMidiSink::sendProgramChange(uint8_t program, uint8_t channel) {
    // No internal handler for program change yet
}

void InternalMidiSink::sendPitchBend(uint16_t bend, uint8_t channel) {
    // No internal handler for pitch bend yet
}

void InternalMidiSink::sendRealTime(uint8_t realtimebyte) {
    if (_onRealTime) _onRealTime(realtimebyte);
}

void InternalMidiSink::sendRaw(const uint8_t* data, size_t length) {
    // No internal handler for raw data
}

bool InternalMidiSink::isReady() const {
    return true;
}

void InternalMidiSink::flush() {
    // Nothing to flush for internal routing
}
