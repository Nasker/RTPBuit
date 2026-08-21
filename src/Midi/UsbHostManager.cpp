#include "Midi/UsbHostManager.hpp"
#include "USBHost_t36.h"

void UsbHostManager::begin(USBHost& usb, MIDIDevice* device) {
    _usb = &usb;
    _devices[0] = device;
    _wasConnected[0] = false;
    _connectionChanged = false;
    _deviceCount = 0;
}

void UsbHostManager::update() {
    for (uint8_t i = 0; i < MAX_DEVICES; i++) {
        if (!_devices[i]) continue;
        bool nowConnected = (bool)(*_devices[i]);
        if (nowConnected != _wasConnected[i]) {
            _connectionChanged = true;
            _wasConnected[i] = nowConnected;
            if (nowConnected) {
                _deviceCount++;
                Serial.print("USB Host MIDI device connected: ");
                Serial.println(getDeviceName(i));
            } else {
                if (_deviceCount > 0) _deviceCount--;
                Serial.println("USB Host MIDI device disconnected");
            }
        }
    }
}

bool UsbHostManager::isDeviceConnected(uint8_t idx) const {
    if (idx >= MAX_DEVICES || !_devices[idx]) return false;
    return (bool)(*_devices[idx]);
}

String UsbHostManager::getDeviceName(uint8_t idx) const {
    if (idx >= MAX_DEVICES || !_devices[idx]) return "None";
    if (!(*_devices[idx])) return "None";
    const uint8_t* p = _devices[idx]->product();
    if (p && p[0]) return String((const char*)p);
    return "USB MIDI Device";
}

String UsbHostManager::getManufacturerName(uint8_t idx) const {
    if (idx >= MAX_DEVICES || !_devices[idx]) return "";
    if (!(*_devices[idx])) return "";
    const uint8_t* m = _devices[idx]->manufacturer();
    if (m && m[0]) return String((const char*)m);
    return "";
}

MIDIDevice* UsbHostManager::getDevice(uint8_t idx) const {
    if (idx >= MAX_DEVICES) return nullptr;
    return _devices[idx];
}

bool UsbHostManager::hasConnectionChanged() {
    bool changed = _connectionChanged;
    _connectionChanged = false;
    return changed;
}
