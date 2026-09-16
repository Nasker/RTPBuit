#include "Midi/UsbHostManager.hpp"
#include "USBHost_t36.h"

void UsbHostManager::begin(USBHost& usb) {
    _usb = &usb;
    _connectionChanged = false;
    _deviceCount = 0;
}

void UsbHostManager::addDevice(MIDIDevice* device, uint8_t idx) {
    if (idx < MAX_DEVICES) {
        _devices[idx] = device;
        _wasConnected[idx] = false;
    }
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
                Serial.printf("USB Host MIDI device connected at index %u: ", i);
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

String UsbHostManager::getDeviceLabel(uint8_t idx) const {
    if (idx >= MAX_DEVICES || !_devices[idx] || !(*_devices[idx])) return "";
    char buf[16];
    snprintf(buf, sizeof(buf), "%04X:%04X",
             _devices[idx]->idVendor(), _devices[idx]->idProduct());
    String label = buf;
    const uint8_t* s = _devices[idx]->serialNumber();
    if (s && s[0]) {
        label += "#";
        label += (const char*)s;
    }
    const uint8_t* p = _devices[idx]->product();
    label += "|";
    label += (p && p[0]) ? String((const char*)p) : String("USB MIDI Device");
    return label;
}

int8_t UsbHostManager::findDeviceByLabel(const char* label) const {
    if (!label || !label[0]) return -1;
    for (uint8_t i = 0; i < MAX_DEVICES; i++) {
        if (!isDeviceConnected(i)) continue;
        if (getDeviceLabel(i) == label) return (int8_t)i;
    }
    return -1;
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
