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
    
    // Flag to indicate if recording is active
    bool _isRecording;
    
    // Current sequence length in ticks
    uint16_t _sequenceLength;
    
public:
    NotesRecorder();
    
    // Start/stop recording
    void startRecording(uint16_t sequenceLength, uint8_t midiChannel);
    void stopRecording();
    bool isRecording() const;
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
    
    // Get quantized position for a tick
    uint16_t quantizeTick(uint32_t tick) const;
    
    // Get recorded notes
    const vector<RTPEventNotePlus>& getRecordedNotes() const;
    void clearRecordedNotes();
    
    // End of sequence handling
    bool isEndOfSequence() const;
    vector<RTPEventNotePlus> dumpRecordedSequence();
    
    // Drum mode sequence mapping
    // Maps recorded notes to sequences in a scene based on note values
    // Returns a map where key=sequence index, value=vector of notes for that sequence
    std::map<uint8_t, vector<RTPEventNotePlus>> dumpDrumSequences();
};
