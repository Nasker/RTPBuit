#pragma once

#include <cstdint>

/**
 * @brief MIDI port identifiers used as bitmask flags
 * 
 * Each port represents a physical or virtual MIDI transport.
 * Values are powers of two so they can be OR'd together for multi-port routing.
 */
enum class MidiPort : uint8_t {
    NONE       = 0x00,
    USB_DEVICE = 0x01,  // USB MIDI device mode (cable to DAW/host)
    USB_HOST   = 0x02,  // USB Host port (external synths/controllers)
    DIN        = 0x04,  // 5-pin DIN via Serial1
    INTERNAL   = 0x08,  // Internal sequencer, automation, clock
    ALL        = 0x0F
};

inline MidiPort operator|(MidiPort a, MidiPort b) {
    return static_cast<MidiPort>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

inline MidiPort operator&(MidiPort a, MidiPort b) {
    return static_cast<MidiPort>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
}

inline MidiPort operator~(MidiPort a) {
    return static_cast<MidiPort>(~static_cast<uint8_t>(a) & 0x0F);
}

inline bool hasPort(MidiPort mask, MidiPort port) {
    return static_cast<uint8_t>(mask & port) != 0;
}

constexpr uint8_t MIDI_PORT_COUNT = 4;

/**
 * @brief Convert a MidiPort single-bit value to an array index (0-3)
 * @return Index: USB_DEVICE=0, USB_HOST=1, DIN=2, INTERNAL=3
 */
inline uint8_t portToIndex(MidiPort port) {
    switch (port) {
        case MidiPort::USB_DEVICE: return 0;
        case MidiPort::USB_HOST:   return 1;
        case MidiPort::DIN:        return 2;
        case MidiPort::INTERNAL:   return 3;
        default:                   return 0;
    }
}

/**
 * @brief Convert an array index (0-3) to a MidiPort value
 */
inline MidiPort indexToPort(uint8_t index) {
    switch (index) {
        case 0: return MidiPort::USB_DEVICE;
        case 1: return MidiPort::USB_HOST;
        case 2: return MidiPort::DIN;
        case 3: return MidiPort::INTERNAL;
        default: return MidiPort::NONE;
    }
}
