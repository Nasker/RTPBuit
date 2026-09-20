#include "Helpers/NotesRecorder.hpp"

NotesRecorder::NotesRecorder() {
    _quantizeStrength = 100;   // Default: hard quantize to the lane's grid
    _isRecording = false;
    _waitingToStart = false;
    _sequenceLength = 0;
    _pulsesPerStep = 6;        // 1/16 at 24 PPQN until the owner syncs us
    _curStep = 0;
    _curPulse = 0;
    _hasSynced = false;
    _loopCompleted = false;
    _currentChannel = 1;       // Default to MIDI channel 1
    _drumMode = false;
    _baseNote = 36;            // Default to C1 (36) as base note for drum mapping
}

uint32_t NotesRecorder::currentPulseAbs() const {
    return (uint32_t)_curStep * _pulsesPerStep + _curPulse;
}

uint32_t NotesRecorder::loopPulses() const {
    return (uint32_t)_sequenceLength * _pulsesPerStep;
}

void NotesRecorder::startRecording(uint16_t sequenceLength, uint8_t midiChannel,
                                   uint8_t pulsesPerStep, uint16_t startStep, uint8_t startPulse) {
    _recordedNotes.clear();
    _activeNotes.clear();

    _sequenceLength = sequenceLength;
    _pulsesPerStep = pulsesPerStep ? pulsesPerStep : 1;
    _currentChannel = midiChannel;
    _curStep = startStep;
    _curPulse = startPulse;
    _hasSynced = true;
    _loopCompleted = false;

    // At the very top of the loop capture starts now; anywhere else we arm
    // and wait for the sequence to wrap so the take is a full, aligned loop.
    bool atLoopStart = (startStep == 0 && startPulse == 0);
    _isRecording = atLoopStart;
    _waitingToStart = !atLoopStart;
}

bool NotesRecorder::isWaiting() const {
    return _waitingToStart;
}

void NotesRecorder::stopRecording() {
    _isRecording = false;
    _waitingToStart = false;

    // Finalize any still-held notes: their length runs to where we are now.
    uint32_t nowAbs = currentPulseAbs();
    for (auto& kv : _activeNotes) {
        ActiveNote& an = kv.second;
        uint32_t span = (nowAbs >= an.onPulseAbs) ? (nowAbs - an.onPulseAbs)
                                                  : (nowAbs + loopPulses() - an.onPulseAbs);
        uint32_t steps = (span + _pulsesPerStep / 2) / _pulsesPerStep;
        an.note.setLength(constrain(steps, 1, 15));
        _recordedNotes.push_back(an.note);
    }
    _activeNotes.clear();
}

uint8_t NotesRecorder::getCurrentChannel() const {
    return _currentChannel;
}

bool NotesRecorder::isRecording() const {
    return _isRecording;
}

void NotesRecorder::setPulsesPerStep(uint8_t pulsesPerStep) {
    _pulsesPerStep = pulsesPerStep ? pulsesPerStep : 1;
}

void NotesRecorder::syncPosition(uint16_t step, uint8_t pulse) {
    bool wrapped = _hasSynced && step == 0 && pulse == 0 && !(_curStep == 0 && _curPulse == 0);
    _curStep = step;
    _curPulse = pulse;
    _hasSynced = true;

    if (_waitingToStart) {
        if (step == 0 && pulse == 0) {
            _waitingToStart = false;
            _isRecording = true;
        }
        return;
    }
    if (_isRecording && wrapped)
        _loopCompleted = true;
}

uint32_t NotesRecorder::quantizePulse(uint32_t pulseAbs) const {
    uint32_t loop = loopPulses();
    if (loop == 0) return 0;
    pulseAbs %= loop;

    // Nearest step boundary (ties round up), then keep (100 - strength)% of
    // the played deviation. Rushed notes therefore land on the step they were
    // aimed at instead of the one before it.
    int32_t nearest = ((int32_t)pulseAbs + _pulsesPerStep / 2) / _pulsesPerStep * _pulsesPerStep;
    int32_t deviation = (int32_t)pulseAbs - nearest;
    int32_t kept = deviation * (100 - (int32_t)_quantizeStrength) / 100;
    int32_t result = nearest + kept;

    result %= (int32_t)loop;
    if (result < 0) result += loop;
    return (uint32_t)result;
}

void NotesRecorder::recordNoteOn(uint8_t note, uint8_t velocity) {
    if (!_isRecording || _waitingToStart) return;

    uint32_t onAbs = quantizePulse(currentPulseAbs());

    RTPEventNotePlus newNote(_currentChannel, true, note, velocity);
    newNote.setEventRead(onAbs / _pulsesPerStep);      // step
    newNote.setMicroOffset(onAbs % _pulsesPerStep);    // pulses into the step
    newNote.setLength(1);
    newNote.setLiteralPitch(true); // Keyboard note: play as-is, skip harmony remapping

    _activeNotes[note] = ActiveNote{newNote, onAbs};
}

void NotesRecorder::recordNoteOff(uint8_t note) {
    if (!_isRecording || _waitingToStart) return;

    auto it = _activeNotes.find(note);
    if (it == _activeNotes.end()) return;

    ActiveNote& an = it->second;
    uint32_t offAbs = currentPulseAbs();
    uint32_t span = (offAbs >= an.onPulseAbs) ? (offAbs - an.onPulseAbs)
                                              : (offAbs + loopPulses() - an.onPulseAbs);
    // Length in steps, rounded to the nearest step; a tap shorter than half a
    // step is still one step.
    uint32_t steps = (span + _pulsesPerStep / 2) / _pulsesPerStep;
    an.note.setLength(constrain(steps, 1, 15));

    _recordedNotes.push_back(an.note);
    _activeNotes.erase(it);
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

bool NotesRecorder::isEndOfSequence() const {
    return _loopCompleted;
}

const vector<RTPEventNotePlus>& NotesRecorder::getRecordedNotes() const {
    return _recordedNotes;
}

void NotesRecorder::clearRecordedNotes() {
    _recordedNotes.clear();
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
