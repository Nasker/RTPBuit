#include "Hardware/Implementations/TeensyMidiOutput.hpp"
#include "Error/Logger.hpp"
#include "Config/MusicConfig.hpp"

TeensyMidiOutput::TeensyMidiOutput(bool enableUsb, bool enableSerial)
    : _usbEnabled(enableUsb), _serialEnabled(enableSerial), _ready(true), _lastErrorTime(0) {
    
    // Initialize hardware serial if enabled
    if (_serialEnabled) {
        Serial1.begin(HardwareConfig::Midi::BAUD_RATE);
        if (!Serial1) {
            _ready = false;
            logError("Failed to initialize hardware serial MIDI");
        }
    }
    
    LOG_INFO_CTX("TeensyMidiOutput initialized (USB: " + String(_usbEnabled ? "ON" : "OFF") + 
                 ", Serial: " + String(_serialEnabled ? "ON" : "OFF") + ")", "TeensyMidiOutput");
}

void TeensyMidiOutput::sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) {
    if (!isReady() || !validateChannel(channel) || !validateNote(note) || !validateVelocity(velocity)) {
        return;
    }
    
    uint8_t status = 0x90 | ((channel - 1) & 0x0F);
    
    if (_usbEnabled) {
        sendUsbMidi(status, note, velocity);
    }
    if (_serialEnabled) {
        sendSerialMidi(status, note, velocity);
    }
}

void TeensyMidiOutput::sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) {
    if (!isReady() || !validateChannel(channel) || !validateNote(note) || !validateVelocity(velocity)) {
        return;
    }
    
    uint8_t status = 0x80 | ((channel - 1) & 0x0F);
    
    if (_usbEnabled) {
        sendUsbMidi(status, note, velocity);
    }
    if (_serialEnabled) {
        sendSerialMidi(status, note, velocity);
    }
}

void TeensyMidiOutput::sendControlChange(uint8_t controller, uint8_t value, uint8_t channel) {
    if (!isReady() || !validateChannel(channel) || !validateController(controller)) {
        return;
    }
    
    uint8_t status = 0xB0 | ((channel - 1) & 0x0F);
    
    if (_usbEnabled) {
        sendUsbMidi(status, controller, value);
    }
    if (_serialEnabled) {
        sendSerialMidi(status, controller, value);
    }
}

void TeensyMidiOutput::sendProgramChange(uint8_t program, uint8_t channel) {
    if (!isReady() || !validateChannel(channel) || !validateProgram(program)) {
        return;
    }
    
    uint8_t status = 0xC0 | ((channel - 1) & 0x0F);
    
    if (_usbEnabled) {
        sendUsbMidi(status, program, 0);
    }
    if (_serialEnabled) {
        sendSerialMidi(status, program, 0);
    }
}

void TeensyMidiOutput::sendPitchBend(uint16_t bend, uint8_t channel) {
    if (!isReady() || !validateChannel(channel) || !validatePitchBend(bend)) {
        return;
    }
    
    uint8_t status = 0xE0 | ((channel - 1) & 0x0F);
    uint8_t lsb = bend & 0x7F;
    uint8_t msb = (bend >> 7) & 0x7F;
    
    if (_usbEnabled) {
        sendUsbMidi(status, lsb, msb);
    }
    if (_serialEnabled) {
        sendSerialMidi(status, lsb, msb);
    }
}

void TeensyMidiOutput::sendRealTime(uint8_t realtimebyte) {
    if (!isReady()) {
        return;
    }
    
    if (_usbEnabled) {
        sendUsbRealTime(realtimebyte);
    }
    if (_serialEnabled) {
        sendSerialRealTime(realtimebyte);
    }
}

void TeensyMidiOutput::sendRaw(const uint8_t* data, size_t length) {
    if (!isReady() || !data || length == 0) {
        return;
    }
    
    if (_usbEnabled) {
        // Send raw MIDI bytes via USB MIDI
        for (size_t i = 0; i < length; i++) {
            usbMIDI.send(data[i], 0, 0, 0, 0); // Send as raw byte with default parameters
        }
    }
    
    if (_serialEnabled) {
        Serial1.write(data, length);
    }
}

bool TeensyMidiOutput::isReady() const {
    return _ready && (_usbEnabled || _serialEnabled);
}

void TeensyMidiOutput::flush() {
    if (_serialEnabled) {
        Serial1.flush();
    }
    // USB MIDI doesn't need explicit flushing
}

void TeensyMidiOutput::sendUsbMidi(uint8_t status, uint8_t data1, uint8_t data2) {
    switch (status & 0xF0) {
        case 0x80:  // Note Off
            usbMIDI.sendNoteOff(data1, data2, (status & 0x0F) + 1);
            break;
        case 0x90:  // Note On
            usbMIDI.sendNoteOn(data1, data2, (status & 0x0F) + 1);
            break;
        case 0xB0:  // Control Change
            usbMIDI.sendControlChange(data1, data2, (status & 0x0F) + 1);
            break;
        case 0xC0:  // Program Change
            usbMIDI.sendProgramChange(data1, (status & 0x0F) + 1);
            break;
        case 0xE0:  // Pitch Bend
            usbMIDI.sendPitchBend(data1 | (data2 << 7), (status & 0x0F) + 1);
            break;
        default:
            logError("Invalid USB MIDI status byte: 0x" + String(status, HEX));
            break;
    }
}

void TeensyMidiOutput::sendSerialMidi(uint8_t status, uint8_t data1, uint8_t data2) {
    Serial1.write(status);
    Serial1.write(data1 & 0x7F);
    Serial1.write(data2 & 0x7F);
}

void TeensyMidiOutput::sendUsbRealTime(uint8_t realtimebyte) {
    usbMIDI.sendRealTime(realtimebyte);
}

void TeensyMidiOutput::sendSerialRealTime(uint8_t realtimebyte) {
    Serial1.write(realtimebyte);
}

bool TeensyMidiOutput::validateChannel(uint8_t channel) {
    if (channel < 1 || channel > HardwareConfig::Midi::CHANNEL_COUNT) {
        logError("Invalid MIDI channel: " + String(channel));
        _lastErrorTime = millis();
        return false;
    }
    return true;
}

bool TeensyMidiOutput::validateNote(uint8_t note) {
    if (note > MusicConfig::Notes::MAX_MIDI_NOTE) {
        logError("Invalid MIDI note: " + String(note));
        _lastErrorTime = millis();
        return false;
    }
    return true;
}

bool TeensyMidiOutput::validateVelocity(uint8_t velocity) {
    if (velocity > MusicConfig::Velocity::MAX_VELOCITY) {
        logError("Invalid MIDI velocity: " + String(velocity));
        _lastErrorTime = millis();
        return false;
    }
    return true;
}

bool TeensyMidiOutput::validateController(uint8_t controller) {
    if (controller > HardwareConfig::Midi::CC_COUNT) {
        logError("Invalid MIDI controller: " + String(controller));
        _lastErrorTime = millis();
        return false;
    }
    return true;
}

bool TeensyMidiOutput::validateProgram(uint8_t program) {
    if (program > HardwareConfig::Midi::CC_COUNT) {
        logError("Invalid MIDI program: " + String(program));
        _lastErrorTime = millis();
        return false;
    }
    return true;
}

bool TeensyMidiOutput::validatePitchBend(uint16_t bend) {
    if (bend > 16383) {  // 14-bit max value
        logError("Invalid pitch bend value: " + String(bend));
        _lastErrorTime = millis();
        return false;
    }
    return true;
}

void TeensyMidiOutput::logError(const String& message) {
    LOG_ERROR_CTX(message, "TeensyMidiOutput");
    _lastErrorTime = millis();
}
