#include "Midi/UsbHostMidiOutput.hpp"
#include "USBHost_t36.h"

void UsbHostMidiOutput::sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) {
    if (_device && *_device) _device->sendNoteOn(note, velocity, channel);
}

void UsbHostMidiOutput::sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) {
    if (_device && *_device) _device->sendNoteOff(note, velocity, channel);
}

void UsbHostMidiOutput::sendControlChange(uint8_t controller, uint8_t value, uint8_t channel) {
    if (_device && *_device) _device->sendControlChange(controller, value, channel);
}

void UsbHostMidiOutput::sendProgramChange(uint8_t program, uint8_t channel) {
    if (_device && *_device) _device->sendProgramChange(program, channel);
}

void UsbHostMidiOutput::sendPitchBend(uint16_t bend, uint8_t channel) {
    if (_device && *_device) _device->sendPitchBend(static_cast<int>(bend) - 8192, channel);
}

void UsbHostMidiOutput::sendRealTime(uint8_t realtimebyte) {
    if (_device && *_device) _device->sendRealTime(realtimebyte);
}

void UsbHostMidiOutput::sendRaw(const uint8_t* data, size_t length) {
    // USB Host MIDI doesn't support arbitrary raw byte sends
    (void)data;
    (void)length;
}

bool UsbHostMidiOutput::isReady() const {
    return _device && *_device;
}

void UsbHostMidiOutput::flush() {
    // No explicit flush needed for USB Host MIDI
}
