#pragma once

#include "Helpers/NotesRecorder.hpp"
#include "Error/Result.hpp"
#include <memory>

/**
 * @brief Manager for recording functionality
 * 
 * Wraps NotesRecorder and provides a clean interface for recording operations.
 * Follows Single Responsibility Principle by focusing only on recording logic.
 */
class RecordingManager {
private:
    NotesRecorder _recorder;
    bool _ready;

public:
    RecordingManager() : _ready(false) {}

    /**
     * @brief Initialize the recording manager
     * @return Result indicating success or failure
     */
    Result<void> initialize() {
        _ready = true;
        return Result<void>::Ok();
    }

    /**
     * @brief Check if manager is ready
     * @return true if ready
     */
    bool isReady() const { return _ready; }

    // Recording control
    void startRecording(uint16_t sequenceLength, uint8_t midiChannel, uint16_t startPosition = 0) {
        _recorder.startRecording(sequenceLength, midiChannel, startPosition);
    }

    void stopRecording() {
        _recorder.stopRecording();
    }

    bool isRecording() const {
        return _recorder.isRecording();
    }

    bool isWaiting() const {
        return _recorder.isWaiting();
    }

    // Note recording
    void recordNoteOn(uint8_t note, uint8_t velocity) {
        _recorder.recordNoteOn(note, velocity);
    }

    void recordNoteOff(uint8_t note) {
        _recorder.recordNoteOff(note);
    }

    // Tick management
    void advanceTick() {
        _recorder.advanceTick();
    }

    void resetTicks() {
        _recorder.resetTicks();
    }

    uint32_t getCurrentTick() const {
        return _recorder.getCurrentTick();
    }

    // Quantization
    void setQuantizeGrid(uint8_t grid) {
        _recorder.setQuantizeGrid(grid);
    }

    uint8_t getQuantizeGrid() const {
        return _recorder.getQuantizeGrid();
    }

    void setQuantizeStrength(uint8_t strength) {
        _recorder.setQuantizeStrength(strength);
    }

    uint8_t getQuantizeStrength() const {
        return _recorder.getQuantizeStrength();
    }

    // Drum mode
    void enableDrumMode(uint8_t baseNote) {
        _recorder.enableDrumMode(baseNote);
    }

    void disableDrumMode() {
        _recorder.disableDrumMode();
    }

    bool isDrumMode() const {
        return _recorder.isDrumMode();
    }

    // Get recorded data
    const vector<RTPEventNotePlus>& getRecordedNotes() const {
        return _recorder.getRecordedNotes();
    }

    void clearRecordedNotes() {
        _recorder.clearRecordedNotes();
    }

    vector<RTPEventNotePlus> dumpRecordedSequence() {
        return _recorder.dumpRecordedSequence();
    }

    std::map<uint8_t, vector<RTPEventNotePlus>> dumpDrumSequences() {
        return _recorder.dumpDrumSequences();
    }

    // Sequence info
    uint16_t getSequenceLength() const {
        return _recorder.getSequenceLength();
    }

    uint8_t getCurrentChannel() const {
        return _recorder.getCurrentChannel();
    }

    bool isEndOfSequence() const {
        return _recorder.isEndOfSequence();
    }

    bool isStartOfSequence() const {
        return _recorder.isStartOfSequence();
    }
};
