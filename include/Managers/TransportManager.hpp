#pragma once

#include "Interfaces/IClockGenerator.hpp"
#include "Interfaces/ISequencer.hpp"
#include "Config/MusicConfig.hpp"
#include "Error/Result.hpp"
#include <Arduino.h>
#include <functional>
#include <memory>

/**
 * @brief Transport control manager
 * 
 * Extracted from BuitDevicesManager to follow Single Responsibility Principle.
 * Handles all transport operations including play/stop, tempo control, recording,
 * and clock synchronization.
 */
class TransportManager {
private:
    std::shared_ptr<IClockGenerator> _clockGenerator;
    std::shared_ptr<ISequencer> _sequencer;
    
    bool _ready;
    mutable uint32_t _lastErrorTime;
    uint8_t _swingAmount;
    uint8_t _quantizeStrength;

public:
    /**
     * @brief Default constructor
     */
    TransportManager();
    
    /**
     * @brief Constructor
     * @param clockGenerator Clock generator interface
     * @param sequencer Sequencer interface
     */
    TransportManager(std::shared_ptr<IClockGenerator> clockGenerator,
                     std::shared_ptr<ISequencer> sequencer);

    /**
     * @brief Initialize the transport manager
     * @return true if initialization successful
     */
    Result<void> initialize();

    /**
     * @brief Check if transport manager is ready
     * @return true if ready
     */
    bool isReady() const;

    // Playback Control
    void play();
    void stop();
    void pause();
    void togglePlayback();
    bool isPlaying() const;

    // Recording Control
    void startRecording();
    void stopRecording();
    void toggleRecording();
    bool isRecording() const;
    bool isWaitingToRecord() const;

    // Tempo Control
    void setBPM(float bpm);
    float getBPM() const;
    void incrementBPM(float delta);
    void tapTempo();
    void clearTapTempo();

    // Clock Synchronization
    void setSyncMode(SyncMode mode);
    SyncMode getSyncMode() const;
    void toggleSyncMode();

    // Swing and Quantization
    void setSwing(int swing);
    int getSwing() const;
    void incrementSwing(int delta);
    
    void setQuantizeStrength(int strength);
    int getQuantizeStrength() const;
    void incrementQuantizeStrength(int delta);

    // Transport State
    struct TransportState {
        bool isPlaying;
        bool isRecording;
        bool isWaiting;
        SyncMode syncMode;
        float bpm;
        int swing;
        int quantizeStrength;
    };
    
    TransportState getState() const;

    // Event Callbacks
    void setPlaybackChangeCallback(std::function<void(bool isPlaying)> callback);
    void setRecordingChangeCallback(std::function<void(bool isRecording, bool isWaiting)> callback);
    void setTempoChangeCallback(std::function<void(float bpm)> callback);
    void setSyncModeChangeCallback(std::function<void(SyncMode mode)> callback);

    // Utility Methods
    void reset();
    String getStatusString() const;
    uint32_t getTimeSinceLastAction() const;

private:
    void logError(const String& message) const;
    void validateBPM(float bpm) const;
    void validateSwing(int swing) const;
    void validateQuantizeStrength(int strength) const;
    
    // Event callbacks
    std::function<void(bool)> _playbackChangeCallback;
    std::function<void(bool, bool)> _recordingChangeCallback;
    std::function<void(float)> _tempoChangeCallback;
    std::function<void(SyncMode)> _syncModeChangeCallback;
};
