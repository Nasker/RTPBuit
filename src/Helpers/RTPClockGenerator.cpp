#include "Helpers/RTPClockGenerator.hpp"

namespace rtp {

RTPClockGenerator::RTPClockGenerator() {
    calculateClockInterval();
}

void RTPClockGenerator::setMode(SyncMode mode) {
    if (_mode == mode) return;
    
    // Stop playback when switching modes
    if (_isRunning) {
        stop();
    }
    
    _mode = mode;
    
    // Reset state
    resetCounter();
    _accumulatorUs = 0;
    _lastClockTimeUs = 0;
}

void RTPClockGenerator::toggleMode() {
    setMode((_mode == SyncMode::External) ? SyncMode::Internal : SyncMode::External);
}

void RTPClockGenerator::start() {
    if (_mode == SyncMode::External) return;  // Only control internal mode
    
    if (!_isRunning) {
        _isRunning = true;
        _pendingStart = true;
        _lastClockTimeUs = micros();
        _accumulatorUs = 0;
    }
}

void RTPClockGenerator::stop() {
    if (_mode == SyncMode::External) return;
    
    if (_isRunning) {
        _isRunning = false;
        _pendingStop = true;
        resetCounter();
    }
}

void RTPClockGenerator::setBPM(float bpm) {
    // Constrain BPM to valid range
    if (bpm < MIN_BPM) bpm = MIN_BPM;
    if (bpm > MAX_BPM) bpm = MAX_BPM;
    
    _bpm = bpm;
    _usingTapTempo = false;  // Manual BPM override clears tap tempo
    calculateClockInterval();
}

void RTPClockGenerator::incrementBPM(float delta) {
    setBPM(_bpm + delta);
}

void RTPClockGenerator::tapTempo() {
    uint32_t now = millis();
    
    // Check for timeout - clear tap history if too much time passed
    if (_lastTapTime > 0 && (now - _lastTapTime) > TAP_TIMEOUT_MS) {
        clearTapTempo();
    }
    
    uint32_t interval = (_lastTapTime > 0) ? (now - _lastTapTime) : 0;
    
    // Only record valid intervals
    if (interval == 0 || isValidTapInterval(interval)) {
        _tapTimes[_tapIndex] = interval;
        _tapIndex = (_tapIndex + 1) % TAP_WINDOW_SIZE;
        if (_tapCount < TAP_WINDOW_SIZE) _tapCount++;
        
        _lastTapTime = now;
        _usingTapTempo = true;
        
        // Calculate BPM from average interval
        if (_tapCount >= 2) {
            uint32_t avgInterval = calculateAverageTapInterval();
            if (avgInterval > 0) {
                float bpm = 60000.0f / avgInterval;
                // Don't constrain here - let setBPM handle it
                _bpm = bpm;
                calculateClockInterval();
            }
        }
    } else {
        // Invalid interval, but still record the tap for next calculation
        _lastTapTime = now;
    }
}

void RTPClockGenerator::clearTapTempo() {
    for (uint8_t i = 0; i < TAP_WINDOW_SIZE; i++) {
        _tapTimes[i] = 0;
    }
    _tapIndex = 0;
    _tapCount = 0;
    _lastTapTime = 0;
    _usingTapTempo = false;
}

bool RTPClockGenerator::isTapTempoActive() const {
    if (!_usingTapTempo || _tapCount < 2) return false;
    
    // Check if tap is still fresh (within timeout)
    uint32_t now = millis();
    return (now - _lastTapTime) < TAP_TIMEOUT_MS;
}

bool RTPClockGenerator::update() {
    // External mode - do nothing, MIDI input handled separately
    if (_mode == SyncMode::External) {
        return false;
    }
    
    // Not running - no clock pulses
    if (!_isRunning) {
        return false;
    }
    
    uint32_t now = micros();
    uint32_t elapsed = now - _lastClockTimeUs;
    _lastClockTimeUs = now;
    
    // Accumulate time
    _accumulatorUs += elapsed;
    
    // Check if clock pulse should fire
    if (_accumulatorUs >= _clockIntervalUs) {
        _accumulatorUs -= _clockIntervalUs;
        
        // Update counter (0-95 for one bar)
        _counter++;
        if (_counter >= 96) {
            _counter = 0;
        }
        
        // Mark clock as pending
        _pendingClock = true;
        
        return true;
    }
    
    return false;
}

// Private methods

void RTPClockGenerator::calculateClockInterval() {
    // MIDI clock runs at 24 PPQN (pulses per quarter note)
    // Interval = 60 seconds / (BPM * 24) = 2,500,000 / BPM microseconds
    _clockIntervalUs = static_cast<uint32_t>(2500000.0f / _bpm);
}

uint32_t RTPClockGenerator::calculateAverageTapInterval() {
    if (_tapCount < 2) return 0;
    
    // Calculate average of recorded intervals (skip first if we have enough)
    uint32_t sum = 0;
    uint8_t validCount = 0;
    uint8_t startIdx = (_tapCount >= TAP_WINDOW_SIZE) ? 0 : 1;  // Skip first incomplete interval
    
    for (uint8_t i = startIdx; i < _tapCount; i++) {
        uint8_t idx = (_tapIndex + TAP_WINDOW_SIZE - _tapCount + i) % TAP_WINDOW_SIZE;
        uint32_t interval = _tapTimes[idx];
        if (interval > 0 && isValidTapInterval(interval)) {
            sum += interval;
            validCount++;
        }
    }
    
    if (validCount == 0) return 0;
    return sum / validCount;
}

bool RTPClockGenerator::isValidTapInterval(uint32_t intervalMs) const {
    return intervalMs >= MIN_TAP_INTERVAL_MS && intervalMs <= MAX_TAP_INTERVAL_MS;
}

void RTPClockGenerator::resetCounter() {
    _counter = 0;
    _accumulatorUs = 0;
}

} // namespace rtp
