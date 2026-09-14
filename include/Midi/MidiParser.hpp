#pragma once

#include <cstdint>
#include "Midi/MidiMessage.hpp"
#include "Midi/MidiPort.hpp"

/**
 * @brief Byte-level MIDI stream parser (state machine)
 *
 * Feeds one byte at a time. When a complete message is assembled,
 * parse() returns true and the result is available via getMessage().
 *
 * Handles:
 *  - Channel voice messages (Note On/Off, CC, Program Change, Pitch Bend)
 *  - Real-time messages (single byte, never interrupt running status)
 *  - Running status
 *  - Ignores System Common / SysEx for now
 */
class MidiParser {
public:
    explicit MidiParser(MidiPort source) : _source(source) {}

    /**
     * @brief Feed one byte into the parser.
     * @return true if a complete MidiMessage is ready (call getMessage()).
     */
    bool parse(uint8_t byte);

    /** @brief Retrieve the last completed message. Valid after parse() returns true. */
    const MidiMessage& getMessage() const { return _msg; }

    /** @brief Reset parser state (e.g. on disconnect). */
    void reset();

private:
    MidiPort  _source;
    MidiMessage _msg {};
    uint8_t   _runningStatus = 0;
    uint8_t   _data1 = 0;
    uint8_t   _expected = 0;   // How many data bytes expected (1 or 2)
    uint8_t   _count = 0;      // Data bytes received so far

    bool _buildMessage(uint8_t d1, uint8_t d2);
};
