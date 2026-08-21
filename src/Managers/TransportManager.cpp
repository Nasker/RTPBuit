#include "Managers/TransportManager.hpp"
#include "Error/Logger.hpp"
#include "Error/ErrorHandler.hpp"
#include "Validation/InputValidator.hpp"
#include <memory>

TransportManager::TransportManager()
    : _clockGenerator(nullptr), _sequencer(nullptr),
      _ready(false), _lastErrorTime(0), _swingAmount(0), _quantizeStrength(50) {
}

TransportManager::TransportManager(std::shared_ptr<IClockGenerator> clockGenerator,
                                   std::shared_ptr<ISequencer> sequencer)
    : _clockGenerator(clockGenerator), _sequencer(sequencer),
      _ready(false), _lastErrorTime(0), _swingAmount(0), _quantizeStrength(50) {
    
    if (!_clockGenerator || !_sequencer) {
        ErrorHandler::handleError(ErrorCode::InvalidPointer, ErrorSeverity::Critical,
                                 "Transport interfaces cannot be null", "TransportManager");
    }
}

Result<void> TransportManager::initialize() {
    if (!_clockGenerator || !_sequencer) {
        return Result<void>::failure(ErrorCode::InvalidPointer, ErrorSeverity::Critical,
                                  "Transport interfaces not available");
    }

    // Initialize clock generator
    _clockGenerator->setBPM(MusicConfig::Timing::DEFAULT_BPM);
    _clockGenerator->setMode(SyncMode::Internal);

    _ready = true;
    LOG_INFO_CTX("TransportManager initialized successfully", "TransportManager");
    return Result<void>::Ok();
}

bool TransportManager::isReady() const {
    return _ready && _clockGenerator && _sequencer;
}

void TransportManager::play() {
    if (!isReady()) return;
    
    _clockGenerator->start();
    _sequencer->play();
    
    LOG_INFO_CTX("Transport started", "TransportManager");
    if (_playbackChangeCallback) {
        _playbackChangeCallback(true);
    }
}

void TransportManager::stop() {
    if (!isReady()) return;
    
    _clockGenerator->stop();
    _sequencer->stop();
    
    LOG_INFO_CTX("Transport stopped", "TransportManager");
    if (_playbackChangeCallback) {
        _playbackChangeCallback(false);
    }
}

void TransportManager::pause() {
    if (!isReady()) return;
    
    _clockGenerator->stop();
    _sequencer->pause();
    
    LOG_INFO_CTX("Transport paused", "TransportManager");
    if (_playbackChangeCallback) {
        _playbackChangeCallback(false);
    }
}

void TransportManager::togglePlayback() {
    if (isPlaying()) {
        stop();
    } else {
        play();
    }
}

bool TransportManager::isPlaying() const {
    return isReady() && _sequencer->isPlaying();
}

void TransportManager::startRecording() {
    if (!isReady()) return;
    
    _sequencer->toggleRecording();
    
    LOG_INFO_CTX("Recording started", "TransportManager");
    if (_recordingChangeCallback) {
        _recordingChangeCallback(true, false);
    }
}

void TransportManager::stopRecording() {
    if (!isReady()) return;
    
    _sequencer->toggleRecording();
    
    LOG_INFO_CTX("Recording stopped", "TransportManager");
    if (_recordingChangeCallback) {
        _recordingChangeCallback(false, false);
    }
}

void TransportManager::toggleRecording() {
    if (isRecording()) {
        stopRecording();
    } else {
        startRecording();
    }
}

bool TransportManager::isRecording() const {
    return isReady() && _sequencer->isRecording();
}

bool TransportManager::isWaitingToRecord() const {
    // This would depend on specific sequencer implementation
    return false; // Placeholder
}

void TransportManager::setBPM(float bpm) {
    if (!isReady()) return;
    
    validateBPM(bpm);
    _clockGenerator->setBPM(bpm);
    
    LOG_DEBUG_CTX("BPM set to: " + String(bpm), "TransportManager");
    if (_tempoChangeCallback) {
        _tempoChangeCallback(bpm);
    }
}

float TransportManager::getBPM() const {
    return isReady() ? _clockGenerator->getBPM() : MusicConfig::Timing::DEFAULT_BPM;
}

void TransportManager::incrementBPM(float delta) {
    setBPM(getBPM() + delta);
}

void TransportManager::tapTempo() {
    if (!isReady()) return;
    
    _clockGenerator->tapTempo();
    
    LOG_DEBUG_CTX("Tap tempo", "TransportManager");
    if (_tempoChangeCallback) {
        _tempoChangeCallback(getBPM());
    }
}

void TransportManager::clearTapTempo() {
    if (!isReady()) return;
    
    _clockGenerator->clearTapTempo();
    
    LOG_DEBUG_CTX("Tap tempo cleared", "TransportManager");
}

void TransportManager::setSyncMode(SyncMode mode) {
    if (!isReady()) return;
    
    _clockGenerator->setMode(mode);
    
    LOG_INFO_CTX("Sync mode set to: " + String(mode == SyncMode::Internal ? "Internal" : "External"),
                 "TransportManager");
    if (_syncModeChangeCallback) {
        _syncModeChangeCallback(mode);
    }
}

SyncMode TransportManager::getSyncMode() const {
    return isReady() ? _clockGenerator->getMode() : SyncMode::Internal;
}

void TransportManager::toggleSyncMode() {
    setSyncMode(getSyncMode() == SyncMode::Internal ? SyncMode::External : SyncMode::Internal);
}

void TransportManager::setSwing(int swing) {
    validateSwing(swing);
    _swingAmount = constrain(swing, MusicConfig::Recording::MIN_SWING, MusicConfig::Recording::MAX_SWING);
    
    LOG_DEBUG_CTX("Swing set to: " + String(_swingAmount), "TransportManager");
}

int TransportManager::getSwing() const {
    return _swingAmount;
}

void TransportManager::incrementSwing(int delta) {
    setSwing(_swingAmount + delta);
}

void TransportManager::setQuantizeStrength(int strength) {
    validateQuantizeStrength(strength);
    _quantizeStrength = constrain(strength, MusicConfig::Recording::MIN_QUANTIZE_STRENGTH, 
                                  MusicConfig::Recording::MAX_QUANTIZE_STRENGTH);
    
    LOG_DEBUG_CTX("Quantize strength set to: " + String(_quantizeStrength), "TransportManager");
}

int TransportManager::getQuantizeStrength() const {
    return _quantizeStrength;
}

void TransportManager::incrementQuantizeStrength(int delta) {
    setQuantizeStrength(_quantizeStrength + delta);
}

TransportManager::TransportState TransportManager::getState() const {
    return {
        .isPlaying = isPlaying(),
        .isRecording = isRecording(),
        .isWaiting = isWaitingToRecord(),
        .syncMode = getSyncMode(),
        .bpm = getBPM(),
        .swing = _swingAmount,
        .quantizeStrength = _quantizeStrength
    };
}

void TransportManager::setPlaybackChangeCallback(std::function<void(bool isPlaying)> callback) {
    _playbackChangeCallback = callback;
}

void TransportManager::setRecordingChangeCallback(std::function<void(bool isRecording, bool isWaiting)> callback) {
    _recordingChangeCallback = callback;
}

void TransportManager::setTempoChangeCallback(std::function<void(float bpm)> callback) {
    _tempoChangeCallback = callback;
}

void TransportManager::setSyncModeChangeCallback(std::function<void(SyncMode mode)> callback) {
    _syncModeChangeCallback = callback;
}

void TransportManager::reset() {
    if (!isReady()) return;
    
    stop();
    setBPM(MusicConfig::Timing::DEFAULT_BPM);
    setSwing(MusicConfig::Recording::DEFAULT_SWING);
    setQuantizeStrength(MusicConfig::Recording::DEFAULT_QUANTIZE_STRENGTH);
    setSyncMode(SyncMode::Internal);
    
    LOG_INFO_CTX("Transport reset to defaults", "TransportManager");
}

String TransportManager::getStatusString() const {
    if (!isReady()) return "NOT READY";
    
    String status = isPlaying() ? "PLAYING" : "STOPPED";
    status += " | ";
    status += String(getBPM(), 1) + " BPM";
    status += " | ";
    status += getSyncMode() == SyncMode::Internal ? "INT" : "EXT";
    
    if (isRecording()) {
        status += " | REC";
    }
    
    return status;
}

uint32_t TransportManager::getTimeSinceLastAction() const {
    return millis() - _lastErrorTime;
}

void TransportManager::logError(const String& message) const {
    LOG_ERROR_CTX(message, "TransportManager");
    _lastErrorTime = millis();
}

void TransportManager::validateBPM(float bpm) const {
    if (!InputValidator::isValidBPM(bpm)) {
        logError("Invalid BPM: " + String(bpm));
    }
}

void TransportManager::validateSwing(int swing) const {
    if (!InputValidator::isValidSwing(swing)) {
        logError("Invalid swing: " + String(swing));
    }
}

void TransportManager::validateQuantizeStrength(int strength) const {
    if (!InputValidator::isValidQuantizeStrength(strength)) {
        logError("Invalid quantize strength: " + String(strength));
    }
}
