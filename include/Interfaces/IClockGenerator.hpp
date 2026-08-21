#pragma once

#include <cstdint>
#include <functional>

/**
 * @brief Clock synchronization modes
 */
enum class SyncMode {
    External,   // Follow MIDI IN clock
    Internal    // Generate clock internally, output MIDI OUT
};

/**
 * @brief Interface for clock generation and synchronization
 * 
 * This interface abstracts clock generation, allowing for different
 * synchronization modes and enabling proper testing.
 */
class IClockGenerator {
public:
    virtual ~IClockGenerator() = default;

    // Mode Control
    virtual void setMode(SyncMode mode) = 0;
    virtual SyncMode getMode() const = 0;
    virtual void toggleMode() = 0;

    // Playback Control
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;

    // BPM Control
    virtual void setBPM(float bpm) = 0;
    virtual float getBPM() const = 0;
    virtual void incrementBPM(float delta) = 0;

    // Tap Tempo
    virtual void tapTempo() = 0;
    virtual void clearTapTempo() = 0;
    virtual bool isTapTempoActive() const = 0;

    // Main Update Loop
    virtual bool update() = 0;  // Returns true if clock pulse should fire

    // Counter Access
    virtual uint8_t getCounter() const = 0;

    // MIDI Output Control (when internal master)
    virtual void setSendMidiRealtime(bool enabled) = 0;
    virtual bool isSendingMidiRealtime() const = 0;

    // Pending Event Checks
    virtual bool shouldSendStart() const = 0;
    virtual bool shouldSendStop() const = 0;
    virtual bool shouldSendClock() const = 0;

    // Clear Pending Flags
    virtual void clearPendingStart() = 0;
    virtual void clearPendingStop() = 0;
    virtual void clearPendingClock() = 0;

    // Event Callbacks
    virtual void setStartCallback(std::function<void()> callback) = 0;
    virtual void setStopCallback(std::function<void()> callback) = 0;
    virtual void setClockCallback(std::function<void()> callback) = 0;

    // Constants
    static constexpr float MIN_BPM = 40.0f;
    static constexpr float MAX_BPM = 240.0f;
    static constexpr float DEFAULT_BPM = 120.0f;
};
