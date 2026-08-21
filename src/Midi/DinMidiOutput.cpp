#include "Midi/DinMidiOutput.hpp"
#include <Arduino.h>

void DinMidiOutput::sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) {
    sendThreeByteMessage(0x90 | ((channel - 1) & 0x0F), note & 0x7F, velocity & 0x7F);
}

void DinMidiOutput::sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) {
    sendThreeByteMessage(0x80 | ((channel - 1) & 0x0F), note & 0x7F, velocity & 0x7F);
}

void DinMidiOutput::sendControlChange(uint8_t controller, uint8_t value, uint8_t channel) {
    sendThreeByteMessage(0xB0 | ((channel - 1) & 0x0F), controller & 0x7F, value & 0x7F);
}

void DinMidiOutput::sendProgramChange(uint8_t program, uint8_t channel) {
    sendTwoByteMessage(0xC0 | ((channel - 1) & 0x0F), program & 0x7F);
}

void DinMidiOutput::sendPitchBend(uint16_t bend, uint8_t channel) {
    uint8_t lsb = bend & 0x7F;
    uint8_t msb = (bend >> 7) & 0x7F;
    sendThreeByteMessage(0xE0 | ((channel - 1) & 0x0F), lsb, msb);
}

void DinMidiOutput::sendRealTime(uint8_t realtimebyte) {
    Serial1.write(realtimebyte);
}

void DinMidiOutput::sendRaw(const uint8_t* data, size_t length) {
    Serial1.write(data, length);
}

bool DinMidiOutput::isReady() const {
    return static_cast<bool>(Serial1);
}

void DinMidiOutput::flush() {
    Serial1.flush();
}

void DinMidiOutput::sendThreeByteMessage(uint8_t status, uint8_t data1, uint8_t data2) {
    Serial1.write(status);
    Serial1.write(data1);
    Serial1.write(data2);
}

void DinMidiOutput::sendTwoByteMessage(uint8_t status, uint8_t data1) {
    Serial1.write(status);
    Serial1.write(data1);
}
