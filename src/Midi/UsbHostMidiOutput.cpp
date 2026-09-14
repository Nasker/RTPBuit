#include "Midi/UsbHostMidiOutput.hpp"
#include "USBHost_t36.h"
#include "Arduino.h"

static void logSendSkip(const char* what, uint8_t idx, MIDIDevice* dev) {
    Serial.printf("[USBHostOut] %s SKIPPED: target=%u, ptr=%s, claimed=%s\n",
                  what, idx, dev ? "set" : "null",
                  (dev && *dev) ? "yes" : "no");
}

void UsbHostMidiOutput::setDevice(MIDIDevice* device, uint8_t idx) {
    if (idx < MAX_DEVICES) _devices[idx] = device;
}

MIDIDevice* UsbHostMidiOutput::getDevice(uint8_t idx) const {
    if (idx >= MAX_DEVICES) return nullptr;
    return _devices[idx];
}

void UsbHostMidiOutput::enqueue(uint8_t kind, uint8_t channel, uint8_t data1, uint8_t data2) {
    uint8_t next = (_qTail + 1) & (QUEUE_SIZE - 1);
    if (next == _qHead) {
        if (++_droppedCount % 64 == 1)
            Serial.printf("[USBHostOut] queue full, message dropped (total %lu)\n",
                          (unsigned long)_droppedCount);
        return;
    }
    _queue[_qTail] = { kind, channel, data1, data2, _targetDevice };
    _qTail = next;
}

void UsbHostMidiOutput::sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) {
    enqueue(0, channel, note, velocity);
}

void UsbHostMidiOutput::sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) {
    enqueue(1, channel, note, velocity);
}

void UsbHostMidiOutput::sendControlChange(uint8_t controller, uint8_t value, uint8_t channel) {
    enqueue(2, channel, controller, value);
}

void UsbHostMidiOutput::sendProgramChange(uint8_t program, uint8_t channel) {
    enqueue(3, channel, program, 0);
}

void UsbHostMidiOutput::sendPitchBend(uint16_t bend, uint8_t channel) {
    enqueue(4, channel, bend & 0x7F, (bend >> 7) & 0x7F);
}

void UsbHostMidiOutput::sendRealTime(uint8_t realtimebyte) {
    enqueue(5, 0, realtimebyte, 0);
}

void UsbHostMidiOutput::dispatchQueued(const QueuedMessage& m, uint8_t devIdx) {
    MIDIDevice* dev = _devices[devIdx];
    switch (m.kind) {
        case 0: dev->sendNoteOn(m.data1, m.data2, m.channel); break;
        case 1: dev->sendNoteOff(m.data1, m.data2, m.channel); break;
        case 2: dev->sendControlChange(m.data1, m.data2, m.channel); break;
        case 3: dev->sendProgramChange(m.data1, m.channel); break;
        case 4: dev->sendPitchBend((int)(m.data1 | (m.data2 << 7)) - 8192, m.channel); break;
        case 5: dev->sendRealTime(m.data1); break;
    }
}

void UsbHostMidiOutput::flush() {
    uint32_t now = micros();
    if (now - _lastFlushUs < FLUSH_INTERVAL_US) return;
    _lastFlushUs = now;

    uint8_t sentPerDevice[MAX_DEVICES] = { 0, 0, 0, 0 };
    bool skipLogged = false;

    while (_qHead != _qTail) {
        const QueuedMessage& m = _queue[_qHead];

        if (m.target < MAX_DEVICES) {
            if (sentPerDevice[m.target] >= MAX_PER_DEVICE_PER_FLUSH) break;
            if (!(_devices[m.target] && *_devices[m.target])) {
                if (!skipLogged) {
                    logSendSkip("queued msg", m.target, _devices[m.target]);
                    skipLogged = true;
                }
                _qHead = (_qHead + 1) & (QUEUE_SIZE - 1);  // drop, device gone
                continue;
            }
            dispatchQueued(m, m.target);
            sentPerDevice[m.target]++;
        } else {
            // Broadcast: send to every connected device that still has quota.
            bool anyConnected = false;
            bool anySent = false;
            for (uint8_t i = 0; i < MAX_DEVICES; i++) {
                if (!(_devices[i] && *_devices[i])) continue;
                anyConnected = true;
                if (sentPerDevice[i] >= MAX_PER_DEVICE_PER_FLUSH) continue;
                dispatchQueued(m, i);
                sentPerDevice[i]++;
                anySent = true;
            }
            if (!anyConnected) {
                _qHead = (_qHead + 1) & (QUEUE_SIZE - 1);  // nothing to send to, drop
                continue;
            }
            if (!anySent) break;  // all connected devices at quota, retry next flush
        }

        _qHead = (_qHead + 1) & (QUEUE_SIZE - 1);
    }
}

void UsbHostMidiOutput::sendRaw(const uint8_t* data, size_t length) {
    (void)data;
    (void)length;
}

bool UsbHostMidiOutput::isReady() const {
    for (uint8_t i = 0; i < MAX_DEVICES; i++)
        if (_devices[i] && *_devices[i]) return true;
    return false;
}
