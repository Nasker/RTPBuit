#pragma once

#include "Interfaces/IMidiOutput.hpp"
#include "Error/Result.hpp"
#include "Config/HardwareConfig.hpp"

/**
 * @brief Teensy-specific MIDI output implementation
 * 
 * Implements the IMidiOutput interface using Teensy's USB MIDI and hardware serial MIDI.
 * Supports both USB MIDI (device mode) and hardware serial MIDI (5-pin DIN).
 */
class TeensyMidiOutput : public IMidiOutput {
private:
    bool _usbEnabled;
    bool _serialEnabled;
    bool _ready;
    uint32_t _lastErrorTime;

public:
    /**
     * @brief Constructor
     * @param enableUsb Enable USB MIDI output
     * @param enableSerial Enable hardware serial MIDI output
     */
    TeensyMidiOutput(bool enableUsb = true, bool enableSerial = true);

    // IMidiOutput interface implementation
    void sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) override;
    void sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) override;
    void sendControlChange(uint8_t controller, uint8_t value, uint8_t channel) override;
    void sendProgramChange(uint8_t program, uint8_t channel) override;
    void sendPitchBend(uint16_t bend, uint8_t channel) override;
    void sendRealTime(uint8_t realtimebyte) override;
    void sendRaw(const uint8_t* data, size_t length) override;
    bool isReady() const override;
    void flush() override;

    // Configuration methods
    void enableUsbMidi(bool enabled) { _usbEnabled = enabled; }
    void enableSerialMidi(bool enabled) { _serialEnabled = enabled; }
    bool isUsbEnabled() const { return _usbEnabled; }
    bool isSerialEnabled() const { return _serialEnabled; }

    // Status methods
    uint32_t getLastErrorTime() const { return _lastErrorTime; }
    void clearError() { _lastErrorTime = 0; }

private:
    void sendUsbMidi(uint8_t status, uint8_t data1, uint8_t data2);
    void sendSerialMidi(uint8_t status, uint8_t data1, uint8_t data2);
    void sendUsbRealTime(uint8_t realtimebyte);
    void sendSerialRealTime(uint8_t realtimebyte);
    bool validateChannel(uint8_t channel);
    bool validateNote(uint8_t note);
    bool validateVelocity(uint8_t velocity);
    bool validateController(uint8_t controller);
    bool validateProgram(uint8_t program);
    bool validatePitchBend(uint16_t bend);
    void logError(const String& message);
};
