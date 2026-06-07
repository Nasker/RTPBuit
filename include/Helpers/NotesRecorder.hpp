#pragma once

#include "Sequencer/RTPEventNotePlus.h"
#include "Arduino.h"
#include <map>
#include <vector>

using namespace std;

class NotesRecorder {
    // Container for active notes - using a single map since we record one channel at a time
    // Key is the note number, value is the RTPEventNotePlus object
    std::map<uint8_t, RTPEventNotePlus> _activeNotes;
    
    // Current MIDI channel being recorded
    uint8_t _currentChannel;
    
    // Drum mode settings
    bool _drumMode;
    uint8_t _baseNote;  // Base note for drum mapping (e.g., C1 = 36)
    
    // Container for completed notes ready to be added to the sequence
    // Using vector instead of queue for random access and sorting capabilities
    vector<RTPEventNotePlus> _recordedNotes;
    
    // High-resolution tick counter for precise timing
    uint32_t _tickCounter;
    
    // Quantization grid resolution (in ticks)
    uint8_t _quantizeGrid;
    
    // Quantization strength 0-100 (snap window as % of grid)
    uint8_t _quantizeStrength;
    
    // Flag to indicate if recording is active
    bool _isRecording;
    
    // Flag to indicate we're waiting to start at position 0
    bool _waitingToStart;
    
    // Current sequence length in ticks
    uint16_t _sequenceLength;
    
public:
    NotesRecorder();
    
    // Start/stop recording
    // startPosition: where we are in the sequence when REC is pressed
    // If not at 0, recording waits until next loop start
    void startRecording(uint16_t sequenceLength, uint8_t midiChannel, uint16_t startPosition = 0);
    void stopRecording();
    bool isRecording() const;
    bool isWaiting() const;  // Armed but waiting for position 0
    uint8_t getCurrentChannel() const;
    
    // Drum mode settings
    void enableDrumMode(uint8_t baseNote);
    void disableDrumMode();
    bool isDrumMode() const;
    uint8_t getBaseNote() const;
    
    // Process incoming notes
    void recordNoteOn(uint8_t note, uint8_t velocity);
    void recordNoteOff(uint8_t note);
    
    // Tick management
    void advanceTick();
    void resetTicks();
    uint32_t getCurrentTick() const;
    
    // Note length management
    void increaseNoteLengths();
    
    // Quantization settings
    void setQuantizeGrid(uint8_t grid);
    uint8_t getQuantizeGrid() const;
    void setQuantizeStrength(uint8_t strength); // 0-100, 0=strict(truncate), 50=nearest(default), 100=max forgiveness
    uint8_t getQuantizeStrength() const;
    
    // Get quantized position for a tick
    uint16_t quantizeTick(uint32_t tick) const;
    
    // Get recorded notes
    const vector<RTPEventNotePlus>& getRecordedNotes() const;
    void clearRecordedNotes();
    
    // Sequence length
    uint16_t getSequenceLength() const;
    
    // End of sequence handling
    bool isEndOfSequence() const;
    bool isStartOfSequence() const;  // True at position 0
    vector<RTPEventNotePlus> dumpRecordedSequence();
    
    // Drum mode sequence mapping
    // Maps recorded notes to sequences in a scene based on note values
    // Returns a map where key=sequence index, value=vector of notes for that sequence
    std::map<uint8_t, vector<RTPEventNotePlus>> dumpDrumSequences();
};
