#pragma once

#include <cstdint>
#include <stddef.h>

/**
 * @brief Interface for MIDI output abstraction
 * 
 * This interface abstracts MIDI output operations, allowing for different
 * implementations (USB MIDI, hardware serial, virtual MIDI, etc.) and
 * enabling proper testing with mock implementations.
 */
class IMidiOutput {
public:
    virtual ~IMidiOutput() = default;

    /**
     * @brief Send a Note On message
     * @param note MIDI note number (0-127)
     * @param velocity Note velocity (0-127)
     * @param channel MIDI channel (1-16)
     */
    virtual void sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) = 0;

    /**
     * @brief Send a Note Off message
     * @param note MIDI note number (0-127)
     * @param velocity Release velocity (0-127, typically 0)
     * @param channel MIDI channel (1-16)
     */
    virtual void sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) = 0;

    /**
     * @brief Send a Control Change (CC) message
     * @param controller CC controller number (0-127)
     * @param value CC value (0-127)
     * @param channel MIDI channel (1-16)
     */
    virtual void sendControlChange(uint8_t controller, uint8_t value, uint8_t channel) = 0;

    /**
     * @brief Send a Program Change message
     * @param program Program number (0-127)
     * @param channel MIDI channel (1-16)
     */
    virtual void sendProgramChange(uint8_t program, uint8_t channel) = 0;

    /**
     * @brief Send a Pitch Bend message
     * @param bend Pitch bend value (0-16383, 8192 is center)
     * @param channel MIDI channel (1-16)
     */
    virtual void sendPitchBend(uint16_t bend, uint8_t channel) = 0;

    /**
     * @brief Send a MIDI Real-Time message
     * @param realtimebyte Real-time message byte (0xF8-0xFF)
     */
    virtual void sendRealTime(uint8_t realtimebyte) = 0;

    /**
     * @brief Send raw MIDI bytes
     * @param data Array of MIDI bytes
     * @param length Number of bytes to send
     */
    virtual void sendRaw(const uint8_t* data, size_t length) = 0;

    /**
     * @brief Check if MIDI output is available/ready
     * @return true if ready to send messages
     */
    virtual bool isReady() const = 0;

    /**
     * @brief Flush any pending MIDI messages
     */
    virtual void flush() = 0;
};
