#pragma once

#include <cstdint>
#include "Interfaces/IMidiOutput.hpp"

// Forward declaration — USBHost_t36.h defines MIDIDevice
class MIDIDevice;

/**
 * IMidiOutput implementation that wraps up to 4 USB Host MIDIDevices.
 * Broadcasts output to all connected devices on the USB_HOST port.
 */
class UsbHostMidiOutput : public IMidiOutput {
public:
    static constexpr uint8_t MAX_DEVICES = 4;
    
    void setDevice(MIDIDevice* device, uint8_t idx = 0);
    MIDIDevice* getDevice(uint8_t idx = 0) const;
    
    /** Set which device to target for subsequent sends. 0xFF = all (broadcast). */
    void setTargetDevice(uint8_t idx) { _targetDevice = idx; }
    uint8_t getTargetDevice() const { return _targetDevice; }

    /** Drip-feed queued messages to the USB devices. Call once per loop(),
        after USBHost::Task(). */
    void flush() override;

private:
    MIDIDevice* _devices[MAX_DEVICES] = { nullptr, nullptr, nullptr, nullptr };
    uint8_t _targetDevice = 0xFF;  // 0xFF = broadcast to all

    // Outgoing queue: USBHost_t36's MIDIDevice::write_packed() spins in a
    // blocking while(1) when both TX buffers are full, and the completion
    // callbacks that free them only run from USBHost::Task() in loop().
    // Burst-sending from inside mUnit.update() can therefore deadlock the
    // main loop. Instead we enqueue here and drip-feed in flush(), called
    // once per loop() iteration after USBHost::Task().
    static constexpr uint8_t QUEUE_SIZE = 128;             // power of two
    static constexpr uint8_t MAX_PER_DEVICE_PER_FLUSH = 2; // <= smallest endpoint capacity (2 x tx_max=2)
    static constexpr uint32_t FLUSH_INTERVAL_US = 1000;    // >= 1 USB full-speed frame

    struct QueuedMessage {
        uint8_t kind;     // 0=NoteOn 1=NoteOff 2=CC 3=ProgramChange 4=PitchBend 5=RealTime
        uint8_t channel;
        uint8_t data1;
        uint8_t data2;
        uint8_t target;   // device index, or 0xFF broadcast
    };

    QueuedMessage _queue[QUEUE_SIZE];
    uint8_t _qHead = 0;
    uint8_t _qTail = 0;
    uint32_t _lastFlushUs = 0;
    uint32_t _droppedCount = 0;

    void enqueue(uint8_t kind, uint8_t channel, uint8_t data1, uint8_t data2);
    void dispatchQueued(const QueuedMessage& m, uint8_t devIdx);

    void sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) override;
    void sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) override;
    void sendControlChange(uint8_t controller, uint8_t value, uint8_t channel) override;
    void sendProgramChange(uint8_t program, uint8_t channel) override;
    void sendPitchBend(uint16_t bend, uint8_t channel) override;
    void sendRealTime(uint8_t realtimebyte) override;
    void sendRaw(const uint8_t* data, size_t length) override;
    bool isReady() const override;
};
