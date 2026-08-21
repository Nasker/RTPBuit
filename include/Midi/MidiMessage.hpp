#pragma once

#include <cstdint>
#include "Midi/MidiPort.hpp"

/**
 * @brief Canonical MIDI message used throughout the routing layer
 * 
 * Every MIDI event — whether from USB, DIN, or generated internally —
 * is normalised into this struct before being handed to the MidiRouter.
 */
struct MidiMessage {
    enum Type : uint8_t {
        NoteOn,
        NoteOff,
        ControlChange,
        ProgramChange,
        PitchBend,
        RealTime,
        SysEx,
        Raw
    };

    Type     type;
    uint8_t  channel;   // 1-16, 0 = N/A (real-time / sysex)
    uint8_t  data1;     // note / CC# / program / RT byte
    uint8_t  data2;     // velocity / value / 0
    MidiPort source;    // originating port

    /**
     * @brief Optional destination override.
     * When set to something other than NONE, the router sends to these
     * ports directly instead of consulting the routing table.
     * Used by the sequencer to force per-track port assignment.
     */
    MidiPort destOverride = MidiPort::NONE;

    /**
     * @brief USB Host device index (0-3) for per-device targeting.
     * 0xFF (default) means broadcast to all connected USB Host devices.
     */
    uint8_t usbHostIndex = 0xFF;
};

/**
 * @brief Bitmask for MidiMessage::Type filtering in route entries
 */
namespace MidiTypeMask {
    constexpr uint8_t NONE           = 0x00;
    constexpr uint8_t NOTE_ON        = (1 << MidiMessage::NoteOn);
    constexpr uint8_t NOTE_OFF       = (1 << MidiMessage::NoteOff);
    constexpr uint8_t CC             = (1 << MidiMessage::ControlChange);
    constexpr uint8_t PROGRAM_CHANGE = (1 << MidiMessage::ProgramChange);
    constexpr uint8_t PITCH_BEND     = (1 << MidiMessage::PitchBend);
    constexpr uint8_t REAL_TIME      = (1 << MidiMessage::RealTime);
    constexpr uint8_t NOTES          = NOTE_ON | NOTE_OFF;
    constexpr uint8_t CHANNEL_VOICE  = NOTES | CC | PROGRAM_CHANGE | PITCH_BEND;
    constexpr uint8_t ALL            = 0xFF;
}
