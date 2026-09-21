#pragma once

#include "Sequencer/RTPEventNotePlus.h"
#include "Arduino.h"
#include <map>
#include <vector>

using namespace std;

// Timing model
// ------------
// The recorder does not keep its own clock. On every 24-PPQN pulse the owner
// calls syncPosition() with the *selected sequence's* own step and pulse
// counters, so capture is phase-locked to the lane's clock divider (1/16,
// 1/8T, ...). Note-ons are stamped in absolute pulses (step*pps + pulse) and
// quantised to the nearest step boundary; the strength setting scales how much
// of the played deviation is kept:
//   100 -> hard snap to the grid (default)
//     0 -> keep the exact played timing (stored as RTPEventNote::microOffset)
// Note length is measured in steps from the on/off pulse distance.
class NotesRecorder {
    struct ActiveNote {
        RTPEventNotePlus note;
        uint32_t onPulseAbs;   // absolute pulse of the (quantised) note-on
    };
    // Key is the note number; we record one channel at a time.
    std::map<uint8_t, ActiveNote> _activeNotes;
    
    // Current MIDI channel being recorded
    uint8_t _currentChannel;
    
    // Drum mode settings
    bool _drumMode;
    uint8_t _baseNote;  // Base note for drum mapping (e.g., C1 = 36)

    // Harmony mode: captures (root, chordType) per step, hard quantized,
    // no note-off, no micro-timing. Used by HARMONY_TRACK sequences.
    bool _harmonyMode;
    
    // Completed notes ready to be dumped into the sequence
    vector<RTPEventNotePlus> _recordedNotes;
    
    // Lane clock: pulses per step (from the sequence's clock divider)
    uint8_t _pulsesPerStep;
    // Last synced position from the sequence
    uint16_t _curStep;
    uint8_t _curPulse;
    bool _hasSynced;
    // Set when the sequence wraps to step 0 while recording (loop complete)
    bool _loopCompleted;
    
    // Quantization strength 0-100 (100 = hard grid, 0 = none)
    uint8_t _quantizeStrength;
    
    // Flag to indicate if recording is active
    bool _isRecording;
    
    // Flag to indicate we're waiting to start at position 0
    bool _waitingToStart;
    
    // Sequence length in steps
    uint16_t _sequenceLength;
    
    uint32_t currentPulseAbs() const;
    uint32_t loopPulses() const;
    // Snap an absolute pulse to the grid according to strength; returns the
    // absolute pulse to store (wrapped into the loop).
    uint32_t quantizePulse(uint32_t pulseAbs) const;
    
public:
    NotesRecorder();
    
    // Start/stop recording. (startStep, startPulse) is where the sequence is
    // when REC is pressed: at (0,0) capture starts immediately, otherwise the
    // recorder arms and waits for the next loop start.
    void startRecording(uint16_t sequenceLength, uint8_t midiChannel,
                        uint8_t pulsesPerStep, uint16_t startStep, uint8_t startPulse);
    void stopRecording();
    bool isRecording() const;
    bool isWaiting() const;  // Armed but waiting for position 0
    uint8_t getCurrentChannel() const;
    
    // Drum mode settings
    void enableDrumMode(uint8_t baseNote);
    void disableDrumMode();
    bool isDrumMode() const;
    uint8_t getBaseNote() const;

    // Harmony mode
    void enableHarmonyMode();
    void disableHarmonyMode();
    bool isHarmonyMode() const;
    // Record a chord change at the current step (hard quantized, no length).
    // root: chromatic index 0-11; chordType: 0-15.
    void recordHarmonyEvent(uint8_t root, uint8_t chordType);
    // Dump with forward-fill: every step without a chord inherits the previous.
    vector<RTPEventNotePlus> dumpHarmonySequence();
    
    // Process incoming notes
    void recordNoteOn(uint8_t note, uint8_t velocity);
    void recordNoteOff(uint8_t note);
    
    // Clock sync: call on every 24-PPQN pulse with the sequence's counters.
    void syncPosition(uint16_t step, uint8_t pulse);
    void setPulsesPerStep(uint8_t pulsesPerStep);
    
    // Quantization settings
    void setQuantizeStrength(uint8_t strength); // 0-100: 100 = hard grid (default), 0 = keep played timing
    uint8_t getQuantizeStrength() const;
    
    // Get recorded notes
    const vector<RTPEventNotePlus>& getRecordedNotes() const;
    void clearRecordedNotes();
    
    // Sequence length
    uint16_t getSequenceLength() const;
    
    // End of sequence handling: true once the sequence has wrapped back to
    // step 0 while recording — the owner should stop and dump.
    bool isEndOfSequence() const;
    vector<RTPEventNotePlus> dumpRecordedSequence();
    
    // Drum mode sequence mapping
    // Maps recorded notes to sequences in a scene based on note values
    // Returns a map where key=sequence index, value=vector of notes for that sequence
    std::map<uint8_t, vector<RTPEventNotePlus>> dumpDrumSequences();
};
