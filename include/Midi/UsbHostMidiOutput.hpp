#pragma once

#include <cstdint>
#include "Interfaces/IMidiOutput.hpp"

// Forward declaration — USBHost_t36.h defines MIDIDevice
class MIDIDevice;

/**
 * IMidiOutput implementation that wraps a USB Host MIDIDevice.
 * The MIDIDevice pointer is set at init and can be updated on hot-plug.
 */
class UsbHostMidiOutput : public IMidiOutput {
    MIDIDevice* _device = nullptr;
public:
    void setDevice(MIDIDevice* device) { _device = device; }
    MIDIDevice* getDevice() const { return _device; }

    void sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) override;
    void sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) override;
    void sendControlChange(uint8_t controller, uint8_t value, uint8_t channel) override;
    void sendProgramChange(uint8_t program, uint8_t channel) override;
    void sendPitchBend(uint16_t bend, uint8_t channel) override;
    void sendRealTime(uint8_t realtimebyte) override;
    void sendRaw(const uint8_t* data, size_t length) override;
    bool isReady() const override;
    void flush() override;
};
