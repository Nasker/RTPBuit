#pragma once

#include <Arduino.h>

namespace rtp {

// Clock source modes
enum class SyncMode {
    External,   // Follow MIDI IN clock
    Internal    // Generate clock internally, output MIDI OUT
};

// Clock generator for internal sync with tap tempo
class RTPClockGenerator {
public:
    // BPM limits
    static constexpr float MIN_BPM = 40.0f;
    static constexpr float MAX_BPM = 240.0f;
    static constexpr float DEFAULT_BPM = 120.0f;
    
    // Tap tempo constants
    static constexpr uint8_t TAP_WINDOW_SIZE = 4;
    static constexpr uint32_t TAP_TIMEOUT_MS = 2000;  // Reset tap after 2 seconds
    static constexpr uint32_t MIN_TAP_INTERVAL_MS = 150;  // 400 BPM max
    static constexpr uint32_t MAX_TAP_INTERVAL_MS = 1500;  // 40 BPM min
    
    RTPClockGenerator();
    
    // Mode control
    void setMode(SyncMode mode);
    SyncMode getMode() const { return _mode; }
    void toggleMode();  // Switch between External/Internal
    
    // Playback control
    void start();
    void stop();
    bool isRunning() const { return _isRunning; }
    
    // BPM control
    void setBPM(float bpm);
    float getBPM() const { return _bpm; }
    void incrementBPM(float delta);
    
    // Tap tempo
    void tapTempo();
    void clearTapTempo();
    bool isTapTempoActive() const;
    
    // Main update - call from loop()
    // Returns true if a clock pulse should fire this update
    bool update();
    
    // Get current tick counter (0-95 for one bar)
    uint8_t getCounter() const { return _counter; }
    
    // MIDI output control (when internal master)
    void setSendMidiRealtime(bool enabled) { _sendMidiRealtime = enabled; }
    bool isSendingMidiRealtime() const { return _sendMidiRealtime; }
    
    // Check if transport events need to be sent
    bool shouldSendStart() const { return _pendingStart; }
    bool shouldSendStop() const { return _pendingStop; }
    bool shouldSendClock() const { return _pendingClock; }
    
    // Clear pending flags after sending
    void clearPendingStart() { _pendingStart = false; }
    void clearPendingStop() { _pendingStop = false; }
    void clearPendingClock() { _pendingClock = false; }
    
private:
    SyncMode _mode = SyncMode::External;
    float _bpm = DEFAULT_BPM;
    uint32_t _clockIntervalUs = 0;  // Microseconds between clock ticks
    
    // Timing
    uint32_t _lastClockTimeUs = 0;
    uint32_t _accumulatorUs = 0;
    
    // State
    bool _isRunning = false;
    uint8_t _counter = 0;
    
    // Tap tempo
    uint32_t _tapTimes[TAP_WINDOW_SIZE] = {0};
    uint8_t _tapIndex = 0;
    uint8_t _tapCount = 0;
    uint32_t _lastTapTime = 0;
    bool _usingTapTempo = false;
    
    // MIDI output
    bool _sendMidiRealtime = true;  // Default to sending when internal
    
    // Pending MIDI events
    bool _pendingStart = false;
    bool _pendingStop = false;
    bool _pendingClock = false;
    
    // Internal methods
    void calculateClockInterval();
    uint32_t calculateAverageTapInterval();
    bool isValidTapInterval(uint32_t intervalMs) const;
    void resetCounter();
};

} // namespace rtp

using RTPClockGenerator = rtp::RTPClockGenerator;
using SyncMode = rtp::SyncMode;
