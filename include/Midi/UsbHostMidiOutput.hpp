#pragma once

#include <cstdint>
#include "Interfaces/IMidiOutput.hpp"

// Forward declaration — USBHost_t36.h defines MIDIDevice
class MIDIDevice;

/**
 * IMidiOutput implementation that wraps up to 4 USB Host MIDIDevices.
 * Broadcasts output to all connected devices on the USB_HOST port.
 */
class UsbHostMidiOutput : public IMidiOutput {
public:
    static constexpr uint8_t MAX_DEVICES = 4;
    
    void setDevice(MIDIDevice* device, uint8_t idx = 0);
    MIDIDevice* getDevice(uint8_t idx = 0) const;
    
    /** Set which device to target for subsequent sends. 0xFF = all (broadcast). */
    void setTargetDevice(uint8_t idx) { _targetDevice = idx; }
    uint8_t getTargetDevice() const { return _targetDevice; }

private:
    MIDIDevice* _devices[MAX_DEVICES] = { nullptr, nullptr, nullptr, nullptr };
    uint8_t _targetDevice = 0xFF;  // 0xFF = broadcast to all

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
