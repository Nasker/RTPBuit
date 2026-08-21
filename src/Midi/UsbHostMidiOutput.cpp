#include "Midi/UsbHostMidiOutput.hpp"
#include "USBHost_t36.h"

void UsbHostMidiOutput::setDevice(MIDIDevice* device, uint8_t idx) {
    if (idx < MAX_DEVICES) _devices[idx] = device;
}

MIDIDevice* UsbHostMidiOutput::getDevice(uint8_t idx) const {
    if (idx >= MAX_DEVICES) return nullptr;
    return _devices[idx];
}

void UsbHostMidiOutput::sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) {
    if (_targetDevice < MAX_DEVICES) {
        if (_devices[_targetDevice] && *_devices[_targetDevice])
            _devices[_targetDevice]->sendNoteOn(note, velocity, channel);
    } else {
        for (uint8_t i = 0; i < MAX_DEVICES; i++)
            if (_devices[i] && *_devices[i]) _devices[i]->sendNoteOn(note, velocity, channel);
    }
}

void UsbHostMidiOutput::sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) {
    if (_targetDevice < MAX_DEVICES) {
        if (_devices[_targetDevice] && *_devices[_targetDevice])
            _devices[_targetDevice]->sendNoteOff(note, velocity, channel);
    } else {
        for (uint8_t i = 0; i < MAX_DEVICES; i++)
            if (_devices[i] && *_devices[i]) _devices[i]->sendNoteOff(note, velocity, channel);
    }
}

void UsbHostMidiOutput::sendControlChange(uint8_t controller, uint8_t value, uint8_t channel) {
    if (_targetDevice < MAX_DEVICES) {
        if (_devices[_targetDevice] && *_devices[_targetDevice])
            _devices[_targetDevice]->sendControlChange(controller, value, channel);
    } else {
        for (uint8_t i = 0; i < MAX_DEVICES; i++)
            if (_devices[i] && *_devices[i]) _devices[i]->sendControlChange(controller, value, channel);
    }
}

void UsbHostMidiOutput::sendProgramChange(uint8_t program, uint8_t channel) {
    if (_targetDevice < MAX_DEVICES) {
        if (_devices[_targetDevice] && *_devices[_targetDevice])
            _devices[_targetDevice]->sendProgramChange(program, channel);
    } else {
        for (uint8_t i = 0; i < MAX_DEVICES; i++)
            if (_devices[i] && *_devices[i]) _devices[i]->sendProgramChange(program, channel);
    }
}

void UsbHostMidiOutput::sendPitchBend(uint16_t bend, uint8_t channel) {
    if (_targetDevice < MAX_DEVICES) {
        if (_devices[_targetDevice] && *_devices[_targetDevice])
            _devices[_targetDevice]->sendPitchBend(static_cast<int>(bend) - 8192, channel);
    } else {
        for (uint8_t i = 0; i < MAX_DEVICES; i++)
            if (_devices[i] && *_devices[i]) _devices[i]->sendPitchBend(static_cast<int>(bend) - 8192, channel);
    }
}

void UsbHostMidiOutput::sendRealTime(uint8_t realtimebyte) {
    if (_targetDevice < MAX_DEVICES) {
        if (_devices[_targetDevice] && *_devices[_targetDevice])
            _devices[_targetDevice]->sendRealTime(realtimebyte);
    } else {
        for (uint8_t i = 0; i < MAX_DEVICES; i++)
            if (_devices[i] && *_devices[i]) _devices[i]->sendRealTime(realtimebyte);
    }
}

void UsbHostMidiOutput::sendRaw(const uint8_t* data, size_t length) {
    (void)data;
    (void)length;
}

bool UsbHostMidiOutput::isReady() const {
    for (uint8_t i = 0; i < MAX_DEVICES; i++)
        if (_devices[i] && *_devices[i]) return true;
    return false;
}

void UsbHostMidiOutput::flush() {
    // No explicit flush needed for USB Host MIDI
}
