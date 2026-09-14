#pragma once

#include "Interfaces/IMidiOutput.hpp"
#include "Config/HardwareConfig.hpp"

/**
 * @brief MIDI output via 5-pin DIN (Serial1)
 * 
 * Wraps Teensy's Serial1 for sending MIDI over the DIN connector.
 * Serial1 must be initialised at 31250 baud before use.
 */
class DinMidiOutput : public IMidiOutput {
public:
    DinMidiOutput() = default;

    void sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) override;
    void sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) override;
    void sendControlChange(uint8_t controller, uint8_t value, uint8_t channel) override;
    void sendProgramChange(uint8_t program, uint8_t channel) override;
    void sendPitchBend(uint16_t bend, uint8_t channel) override;
    void sendRealTime(uint8_t realtimebyte) override;
    void sendRaw(const uint8_t* data, size_t length) override;
    bool isReady() const override;
    void flush() override;

private:
    void sendThreeByteMessage(uint8_t status, uint8_t data1, uint8_t data2);
    void sendTwoByteMessage(uint8_t status, uint8_t data1);
};
