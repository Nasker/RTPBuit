#pragma once

#include <cstdint>
#include "Arduino.h"

class MIDIDevice;
class USBHost;

/**
 * Manages USB Host MIDI device enumeration and connection status.
 * Wraps one or more statically-declared MIDIDevice instances and
 * provides hot-plug detection via polling.
 *
 * Teensy 4.1 supports up to 4 USB devices via a hub, but MIDIDevice
 * instances must be declared statically. Start with 1, expand later.
 */
class UsbHostManager {
public:
    static constexpr uint8_t MAX_DEVICES = 1;  // Expand to 2-4 if hub support needed

    void begin(USBHost& usb, MIDIDevice* device);
    void update();  // Call in loop() to detect connect/disconnect

    bool isDeviceConnected(uint8_t idx = 0) const;
    String getDeviceName(uint8_t idx = 0) const;
    String getManufacturerName(uint8_t idx = 0) const;
    MIDIDevice* getDevice(uint8_t idx = 0) const;
    uint8_t getDeviceCount() const { return _deviceCount; }
    bool hasConnectionChanged();  // Returns true once per connect/disconnect event

private:
    MIDIDevice* _devices[MAX_DEVICES] = { nullptr };
    USBHost* _usb = nullptr;
    uint8_t _deviceCount = 0;
    bool _wasConnected[MAX_DEVICES] = { false };
    bool _connectionChanged = false;
};
