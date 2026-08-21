#include "Midi/UsbDeviceMidiOutput.hpp"
#include <Arduino.h>

void UsbDeviceMidiOutput::sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) {
    usbMIDI.sendNoteOn(note, velocity, channel);
}

void UsbDeviceMidiOutput::sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) {
    usbMIDI.sendNoteOff(note, velocity, channel);
}

void UsbDeviceMidiOutput::sendControlChange(uint8_t controller, uint8_t value, uint8_t channel) {
    usbMIDI.sendControlChange(controller, value, channel);
}

void UsbDeviceMidiOutput::sendProgramChange(uint8_t program, uint8_t channel) {
    usbMIDI.sendProgramChange(program, channel);
}

void UsbDeviceMidiOutput::sendPitchBend(uint16_t bend, uint8_t channel) {
    usbMIDI.sendPitchBend(bend, channel);
}

void UsbDeviceMidiOutput::sendRealTime(uint8_t realtimebyte) {
    usbMIDI.sendRealTime(realtimebyte);
}

void UsbDeviceMidiOutput::sendRaw(const uint8_t* data, size_t length) {
    for (size_t i = 0; i < length; i++) {
        usbMIDI.send(data[i], 0, 0, 0, 0);
    }
}

bool UsbDeviceMidiOutput::isReady() const {
    return true;
}

void UsbDeviceMidiOutput::flush() {
    // USB MIDI doesn't need explicit flushing
}
