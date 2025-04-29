#include "Helpers/NotesRecorder.hpp"

NotesRecorder::NotesRecorder() {
    _tickCounter = 0;
    _quantizeGrid = 4; // Default to 16th note quantization (assuming 96 PPQN)
    _isRecording = false;
    _sequenceLength = 0;
    _currentChannel = 1; // Default to MIDI channel 1
    _drumMode = false;
    _baseNote = 36; // Default to C1 (36) as base note for drum mapping
}

void NotesRecorder::startRecording(uint16_t sequenceLength, uint8_t midiChannel) {
    // Clear any previously recorded notes
    _recordedNotes.clear();
    
    // Clear any active notes
    _activeNotes.clear();
    
    _sequenceLength = sequenceLength;
    _tickCounter = 0;
    _currentChannel = midiChannel;
    _isRecording = true;
}

void NotesRecorder::stopRecording() {
    _isRecording = false;
    
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

void NotesRecorder::recordNoteOn(uint8_t note, uint8_t velocity) {
    if (!_isRecording) return;
    
    // Create a new note with the current quantized position
    uint16_t quantizedPosition = quantizeTick(_tickCounter);
    
    // Create the note (state true for note-on)
    RTPEventNotePlus newNote(_currentChannel, true, note, velocity);
    newNote.setEventRead(quantizedPosition);
    newNote.setLength(1); // Start with minimum length
    
    // Store in active notes map using the note number as the key
    _activeNotes[note] = newNote;
}

void NotesRecorder::recordNoteOff(uint8_t note) {
    if (!_isRecording) return;
    
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

uint16_t NotesRecorder::quantizeTick(uint32_t tick) const {
    // Quantize the tick to the nearest grid position
    // This assumes a certain PPQN (Pulses Per Quarter Note) value
    
    // Calculate the nearest grid position
    uint16_t position = (tick / _quantizeGrid) * _quantizeGrid;
    
    // Ensure position is within sequence bounds
    return position % _sequenceLength;
}

const vector<RTPEventNotePlus>& NotesRecorder::getRecordedNotes() const {
    return _recordedNotes;
}

void NotesRecorder::clearRecordedNotes() {
    _recordedNotes.clear();
}

bool NotesRecorder::isEndOfSequence() const {
    // Check if we've reached the end of the sequence
    return (_tickCounter % _sequenceLength) == 0 && _tickCounter > 0;
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
        uint8_t noteValue = note.getNote();
        
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
