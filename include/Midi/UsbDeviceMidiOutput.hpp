#pragma once

#include "Interfaces/IMidiOutput.hpp"

/**
 * @brief MIDI output via USB Device mode (usbMIDI)
 * 
 * Wraps Teensy's built-in usbMIDI interface for sending MIDI
 * to the connected DAW / computer.
 */
class UsbDeviceMidiOutput : public IMidiOutput {
public:
    UsbDeviceMidiOutput() = default;

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
