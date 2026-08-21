#include "Midi/MidiParser.hpp"

void MidiParser::reset() {
    _runningStatus = 0;
    _data1 = 0;
    _expected = 0;
    _count = 0;
}

bool MidiParser::parse(uint8_t byte) {
    // ---- Real-time (0xF8-0xFF): single byte, never interrupts running status ----
    if (byte >= 0xF8) {
        _msg = { MidiMessage::RealTime, 0, byte, 0, _source };
        return true;
    }

    // ---- System Common (0xF0-0xF7): cancel running status, ignore for now ----
    if (byte >= 0xF0) {
        _runningStatus = 0;
        _count = 0;
        _expected = 0;
        return false;
    }

    // ---- Status byte (0x80-0xEF) ----
    if (byte & 0x80) {
        _runningStatus = byte;
        _count = 0;

        uint8_t hi = byte & 0xF0;
        switch (hi) {
            case 0xC0:  // Program Change  (1 data byte)
            case 0xD0:  // Channel Pressure (1 data byte)
                _expected = 1;
                break;
            default:    // Note On/Off, CC, Pitch Bend (2 data bytes)
                _expected = 2;
                break;
        }
        return false;
    }

    // ---- Data byte (0x00-0x7F) ----
    if (_runningStatus == 0) return false;  // No status yet — discard

    if (_count == 0) {
        _data1 = byte;
        _count = 1;
        if (_expected == 1) {
            // 1-byte message complete
            _count = 0;
            return _buildMessage(_data1, 0);
        }
        return false;
    }

    // Second data byte — message complete
    _count = 0;
    return _buildMessage(_data1, byte);
}

bool MidiParser::_buildMessage(uint8_t d1, uint8_t d2) {
    uint8_t hi = _runningStatus & 0xF0;
    uint8_t ch = (_runningStatus & 0x0F) + 1;  // MIDI channels 1-16

    switch (hi) {
        case 0x90:  // Note On (velocity 0 = Note Off)
            _msg = { d2 == 0 ? MidiMessage::NoteOff : MidiMessage::NoteOn,
                     ch, d1, d2, _source };
            return true;
        case 0x80:  // Note Off
            _msg = { MidiMessage::NoteOff, ch, d1, d2, _source };
            return true;
        case 0xB0:  // Control Change
            _msg = { MidiMessage::ControlChange, ch, d1, d2, _source };
            return true;
        case 0xC0:  // Program Change
            _msg = { MidiMessage::ProgramChange, ch, d1, 0, _source };
            return true;
        case 0xE0:  // Pitch Bend
            _msg = { MidiMessage::PitchBend, ch, d1, d2, _source };
            return true;
        default:
            return false;
    }
}
