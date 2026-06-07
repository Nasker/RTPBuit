#include "Helpers/NotesRecorder.hpp"

NotesRecorder::NotesRecorder() {
    _tickCounter = 0;
    _quantizeGrid = 1; // 1 tick = 1 step (grid ticks are already quantized to steps)
    _quantizeStrength = 50; // Default: nearest grid (100% = snap to nearest)
    _isRecording = false;
    _waitingToStart = false;
    _sequenceLength = 0;
    _currentChannel = 1; // Default to MIDI channel 1
    _drumMode = false;
    _baseNote = 36; // Default to C1 (36) as base note for drum mapping
}

void NotesRecorder::startRecording(uint16_t sequenceLength, uint8_t midiChannel, uint16_t startPosition) {
    // Clear any previously recorded notes
    _recordedNotes.clear();
    
    // Clear any active notes
    _activeNotes.clear();
    
    _sequenceLength = sequenceLength;
    // Start one tick behind so that after the first advanceTick() increment,
    // we're exactly at startPosition. This ensures notes at position 0 quantize to 0.
    _tickCounter = (startPosition == 0) ? (_sequenceLength - 1) : (startPosition - 1);
    _currentChannel = midiChannel;
    
    // If not at position 0, wait until we loop back to start
    if (startPosition == 0) {
        _isRecording = true;
        _waitingToStart = false;
    } else {
        _isRecording = false;  // Not actually recording yet
        _waitingToStart = true; // Armed, waiting for position 0
    }
}

bool NotesRecorder::isWaiting() const {
    return _waitingToStart;
}

void NotesRecorder::stopRecording() {
    _isRecording = false;
    _waitingToStart = false;
    
    // Finalize any still-active notes
    for (auto it = _activeNotes.begin(); it != _activeNotes.end(); ++it) {
        // Add the note to recorded notes with current length
        _recordedNotes.push_back(it->second);
    }
    
    // Clear active notes
    _activeNotes.clear();
}

uint8_t NotesRecorder::getCurrentChannel() const {
    return _currentChannel;
}

bool NotesRecorder::isRecording() const {
    return _isRecording;
}

bool NotesRecorder::isStartOfSequence() const {
    if (_sequenceLength == 0) return false;
    return (_tickCounter % _sequenceLength) == 0;
}

void NotesRecorder::recordNoteOn(uint8_t note, uint8_t velocity) {
    // Only record when actually active (not when waiting to start)
    if (!_isRecording || _waitingToStart) return;
    
    // Create a new note with the current quantized position
    uint16_t quantizedPosition = quantizeTick(_tickCounter);
    
    // Create the note (state true for note-on)
    RTPEventNotePlus newNote(_currentChannel, true, note, velocity);
    newNote.setEventRead(quantizedPosition);
    newNote.setLength(1); // Start with minimum length, grows on each tick
    newNote.setLiteralPitch(true); // Keyboard note: play as-is, skip harmony remapping
    
    // Store in active notes map using the note number as the key
    _activeNotes[note] = newNote;
}

void NotesRecorder::recordNoteOff(uint8_t note) {
    // Only process when actually active (not when waiting to start)
    if (!_isRecording || _waitingToStart) return;
    
    // Find the note in the active notes map
    auto it = _activeNotes.find(note);
    
    if (it != _activeNotes.end()) {
        // Calculate the quantized length based on the difference between
        // the note-on position and the current quantized position
        uint16_t noteOnPos = it->second.getEventRead();
        uint16_t noteOffPos = quantizeTick(_tickCounter);
        
        // Calculate length (handle wrap-around if needed)
        uint16_t length;
        if (noteOffPos >= noteOnPos) {
            length = noteOffPos - noteOnPos + 1;
        } else {
            length = (_sequenceLength - noteOnPos) + noteOffPos + 1;
        }
        
        // Ensure minimum length
        if (length < 1) length = 1;
        
        // Set the final length
        it->second.setLength(length);
        
        // Add to recorded notes
        _recordedNotes.push_back(it->second);
        
        // Remove from active notes
        _activeNotes.erase(it);
    }
}

void NotesRecorder::advanceTick() {
    // If waiting to start, check if we've reached position 0
    if (_waitingToStart) {
        _tickCounter++;
        if (isStartOfSequence()) {
            // Transition from waiting to active recording
            _waitingToStart = false;
            _isRecording = true;
        }
        return;
    }
    
    // Normal active recording
    if (!_isRecording) return;
    
    _tickCounter++;
    
    // Increase length of all active notes
    increaseNoteLengths();
}

void NotesRecorder::resetTicks() {
    _tickCounter = 0;
}

uint32_t NotesRecorder::getCurrentTick() const {
    return _tickCounter;
}

void NotesRecorder::increaseNoteLengths() {
    // This function is called on each tick to increase the length of active notes
    // Similar to decreaseTimeToLive in NotesPlayer, but increasing instead
    
    for (auto& notePair : _activeNotes) {
        // Increase the length of the note
        uint8_t currentLength = notePair.second.getLength();
        notePair.second.setLength(currentLength + 1);
    }
}

void NotesRecorder::setQuantizeGrid(uint8_t grid) {
    _quantizeGrid = grid;
}

uint8_t NotesRecorder::getQuantizeGrid() const {
    return _quantizeGrid;
}

void NotesRecorder::setQuantizeStrength(uint8_t strength) {
    _quantizeStrength = constrain(strength, 0, 100);
}

uint8_t NotesRecorder::getQuantizeStrength() const {
    return _quantizeStrength;
}

uint16_t NotesRecorder::getSequenceLength() const {
    return _sequenceLength;
}

uint16_t NotesRecorder::quantizeTick(uint32_t tick) const {
    if (_sequenceLength == 0) return 0;
    
    // Wrap tick to sequence bounds first
    tick = tick % _sequenceLength;
    
    // Find previous and next grid positions
    uint16_t prevGrid = (tick / _quantizeGrid) * _quantizeGrid;
    uint16_t nextGrid = (prevGrid + _quantizeGrid) % _sequenceLength;
    
    // Calculate distances (handling wrap-around)
    uint16_t distPrev = (tick >= prevGrid) ? (tick - prevGrid) : (tick + _sequenceLength - prevGrid);
    uint16_t distNext = (nextGrid >= tick) ? (nextGrid - tick) : (nextGrid + _sequenceLength - tick);
    
    // Determine nearest grid
    uint16_t nearestGrid = (distPrev <= distNext) ? prevGrid : nextGrid;
    uint16_t distNearest = min(distPrev, distNext);
    
    // Apply strength threshold: snap if within window, else use conservative (prevGrid)
    // Strength 0: always use prevGrid (truncate down)
    // Strength 50: snap if within 50% of grid = nearest (normal feel)
    // Strength 100: snap if within 100% of grid = always nearest (max forgiveness)
    uint16_t threshold = (_quantizeGrid * _quantizeStrength) / 100;
    
    if (distNearest <= threshold) {
        return nearestGrid;
    } else {
        // In the "dead zone" - fall back to conservative (previous grid)
        return prevGrid;
    }
}

const vector<RTPEventNotePlus>& NotesRecorder::getRecordedNotes() const {
    return _recordedNotes;
}

void NotesRecorder::clearRecordedNotes() {
    _recordedNotes.clear();
}

bool NotesRecorder::isEndOfSequence() const {
    if (_sequenceLength == 0) return false;
    // Trigger at the LAST tick of the sequence (position sequenceLength-1)
    // This ensures dump happens at the END of the loop, not at start of next loop
    return (_tickCounter % _sequenceLength) == (_sequenceLength - 1);
}

vector<RTPEventNotePlus> NotesRecorder::dumpRecordedSequence() {
    // Return a copy of the recorded notes and clear the internal buffer
    vector<RTPEventNotePlus> notes = _recordedNotes;
    _recordedNotes.clear();
    return notes;
}

void NotesRecorder::enableDrumMode(uint8_t baseNote) {
    _drumMode = true;
    _baseNote = baseNote;
}

void NotesRecorder::disableDrumMode() {
    _drumMode = false;
}

bool NotesRecorder::isDrumMode() const {
    return _drumMode;
}

uint8_t NotesRecorder::getBaseNote() const {
    return _baseNote;
}

std::map<uint8_t, vector<RTPEventNotePlus>> NotesRecorder::dumpDrumSequences() {
    // Create a map to hold notes for each sequence
    std::map<uint8_t, vector<RTPEventNotePlus>> sequenceMap;
    
    // Process each recorded note
    for (const auto& note : _recordedNotes) {
        // Calculate sequence index by subtracting base note from note value
        uint8_t noteValue = note.getEventNote();
        
        // Only process notes at or above the base note
        if (noteValue >= _baseNote) {
            uint8_t sequenceIndex = noteValue - _baseNote;
            
            // Add the note to the appropriate sequence
            sequenceMap[sequenceIndex].push_back(note);
        }
    }
    
    // Clear the recorded notes
    _recordedNotes.clear();
    
    return sequenceMap;
}
