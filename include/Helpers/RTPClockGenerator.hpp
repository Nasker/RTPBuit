#pragma once

#include <Arduino.h>
#include <functional>
#include "Interfaces/IClockGenerator.hpp"

namespace rtp {

// Clock generator for internal sync with tap tempo
class RTPClockGenerator : public IClockGenerator {
public:
    // BPM limits (shadows IClockGenerator constexpr — same values)
    static constexpr float MIN_BPM = 40.0f;
    static constexpr float MAX_BPM = 240.0f;
    static constexpr float DEFAULT_BPM = 120.0f;
    
    // Tap tempo constants
    static constexpr uint8_t TAP_WINDOW_SIZE = 4;
    static constexpr uint32_t TAP_TIMEOUT_MS = 2000;
    static constexpr uint32_t MIN_TAP_INTERVAL_MS = 150;
    static constexpr uint32_t MAX_TAP_INTERVAL_MS = 1500;
    
    RTPClockGenerator();
    
    // Mode control
    void setMode(SyncMode mode) override;
    SyncMode getMode() const override { return _mode; }
    void toggleMode() override;
    
    // Playback control
    void start() override;
    void stop() override;
    bool isRunning() const override { return _isRunning; }
    
    // BPM control
    void setBPM(float bpm) override;
    float getBPM() const override { return _bpm; }
    void incrementBPM(float delta) override;
    
    // Tap tempo
    void tapTempo() override;
    void clearTapTempo() override;
    bool isTapTempoActive() const override;
    
    // Main update - call from loop()
    bool update() override;
    
    // Counter
    uint8_t getCounter() const override { return _counter; }
    
    // MIDI output control
    void setSendMidiRealtime(bool enabled) override { _sendMidiRealtime = enabled; }
    bool isSendingMidiRealtime() const override { return _sendMidiRealtime; }
    
    // Pending event checks
    bool shouldSendStart() const override { return _pendingStart; }
    bool shouldSendStop() const override { return _pendingStop; }
    bool shouldSendClock() const override { return _pendingClock; }
    
    // Clear pending flags
    void clearPendingStart() override { _pendingStart = false; }
    void clearPendingStop() override { _pendingStop = false; }
    void clearPendingClock() override { _pendingClock = false; }
    
    // Callbacks
    void setStartCallback(std::function<void()> callback) override { _startCallback = callback; }
    void setStopCallback(std::function<void()> callback) override { _stopCallback = callback; }
    void setClockCallback(std::function<void()> callback) override { _clockCallback = callback; }
    
private:
    SyncMode _mode = SyncMode::External;
    float _bpm = DEFAULT_BPM;
    uint32_t _clockIntervalUs = 0;
    
    uint32_t _lastClockTimeUs = 0;
    uint32_t _accumulatorUs = 0;
    
    bool _isRunning = false;
    uint8_t _counter = 0;
    
    uint32_t _tapTimes[TAP_WINDOW_SIZE] = {0};
    uint8_t _tapIndex = 0;
    uint8_t _tapCount = 0;
    uint32_t _lastTapTime = 0;
    bool _usingTapTempo = false;
    
    bool _sendMidiRealtime = true;
    
    bool _pendingStart = false;
    bool _pendingStop = false;
    bool _pendingClock = false;
    
    std::function<void()> _startCallback;
    std::function<void()> _stopCallback;
    std::function<void()> _clockCallback;
    
    void calculateClockInterval();
    uint32_t calculateAverageTapInterval();
    bool isValidTapInterval(uint32_t intervalMs) const;
    void resetCounter();
};

} // namespace rtp

using RTPClockGenerator = rtp::RTPClockGenerator;
