#include "Midi/MidiRouter.hpp"

MidiRouter::MidiRouter() : _routeCount(0) {
    for (uint8_t i = 0; i < MIDI_PORT_COUNT; i++) {
        _outputs[i] = nullptr;
    }
    for (uint8_t i = 0; i < MAX_ROUTES; i++) {
        _routes[i] = {MidiPort::NONE, MidiPort::NONE, MidiTypeMask::NONE, 0, 0, false};
    }
}

void MidiRouter::setOutput(MidiPort port, IMidiOutput* output) {
    _outputs[portToIndex(port)] = output;
}

IMidiOutput* MidiRouter::getOutput(MidiPort port) const {
    return _outputs[portToIndex(port)];
}

bool MidiRouter::addRoute(MidiPort source, MidiPort dest,
                           uint8_t msgTypeMask,
                           uint8_t channelMin, uint8_t channelMax) {
    if (_routeCount >= MAX_ROUTES) return false;

    _routes[_routeCount] = {source, dest, msgTypeMask, channelMin, channelMax, true};
    _routeCount++;
    return true;
}

void MidiRouter::removeRoute(uint8_t index) {
    if (index >= _routeCount) return;

    // Shift entries down
    for (uint8_t i = index; i < _routeCount - 1; i++) {
        _routes[i] = _routes[i + 1];
    }
    _routes[_routeCount - 1] = {MidiPort::NONE, MidiPort::NONE, MidiTypeMask::NONE, 0, 0, false};
    _routeCount--;
}

void MidiRouter::clearRoutes() {
    for (uint8_t i = 0; i < MAX_ROUTES; i++) {
        _routes[i] = {MidiPort::NONE, MidiPort::NONE, MidiTypeMask::NONE, 0, 0, false};
    }
    _routeCount = 0;
}

void MidiRouter::setDefaultRoutes() {
    clearRoutes();

    // Internal → USB Device + DIN  (sequencer output goes to both)
    addRoute(MidiPort::INTERNAL, MidiPort::USB_DEVICE | MidiPort::DIN, MidiTypeMask::ALL);

    // USB Device → Internal  (incoming notes, CC, realtime from DAW)
    addRoute(MidiPort::USB_DEVICE, MidiPort::INTERNAL,
             MidiTypeMask::NOTES | MidiTypeMask::CC | MidiTypeMask::REAL_TIME);

    // USB Host → Internal  (incoming notes, CC from external controllers/synths)
    addRoute(MidiPort::USB_HOST, MidiPort::INTERNAL,
             MidiTypeMask::NOTES | MidiTypeMask::CC);

    // DIN → Internal  (real-time clock from external gear)
    addRoute(MidiPort::DIN, MidiPort::INTERNAL, MidiTypeMask::REAL_TIME);
}

void MidiRouter::enableRoute(uint8_t index, bool enabled) {
    if (index < _routeCount) {
        _routes[index].enabled = enabled;
    }
}

void MidiRouter::route(const MidiMessage& msg) {
    // If the message has a destination override, bypass the routing table
    if (msg.destOverride != MidiPort::NONE) {
        for (uint8_t i = 0; i < MIDI_PORT_COUNT; i++) {
            MidiPort port = indexToPort(i);
            if (hasPort(msg.destOverride, port) && port != msg.source) {
                sendToPort(port, msg);
            }
        }
        return;
    }

    // Consult the routing table
    for (uint8_t i = 0; i < _routeCount; i++) {
        if (_routes[i].matches(msg)) {
            // Send to each destination port in the mask (skip source to avoid echo)
            for (uint8_t p = 0; p < MIDI_PORT_COUNT; p++) {
                MidiPort port = indexToPort(p);
                if (hasPort(_routes[i].destMask, port) && port != msg.source) {
                    sendToPort(port, msg);
                }
            }
        }
    }
}

void MidiRouter::sendToPort(MidiPort port, const MidiMessage& msg) {
    IMidiOutput* output = _outputs[portToIndex(port)];
    if (output && output->isReady()) {
        dispatchToOutput(output, msg);
    }
}

void MidiRouter::dispatchToOutput(IMidiOutput* output, const MidiMessage& msg) {
    switch (msg.type) {
        case MidiMessage::NoteOn:
            output->sendNoteOn(msg.data1, msg.data2, msg.channel);
            break;
        case MidiMessage::NoteOff:
            output->sendNoteOff(msg.data1, msg.data2, msg.channel);
            break;
        case MidiMessage::ControlChange:
            output->sendControlChange(msg.data1, msg.data2, msg.channel);
            break;
        case MidiMessage::ProgramChange:
            output->sendProgramChange(msg.data1, msg.channel);
            break;
        case MidiMessage::PitchBend:
            output->sendPitchBend(
                static_cast<uint16_t>(msg.data1) | (static_cast<uint16_t>(msg.data2) << 7),
                msg.channel);
            break;
        case MidiMessage::RealTime:
            output->sendRealTime(msg.data1);
            break;
        case MidiMessage::Raw:
        case MidiMessage::SysEx: {
            uint8_t raw[] = {msg.data1, msg.data2};
            output->sendRaw(raw, 2);
            break;
        }
    }
}
