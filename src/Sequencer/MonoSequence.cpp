#include "MonoSequence.hpp"
#include "ReMap.hpp"
#include "RTPChordMatrix.h"

MonoSequence::MonoSequence(int midiChannel, int NEvents, int type, int baseNote, NotesPlayer& notesPlayer, MusicManager& musicManager):
  RTPEventNoteSequence(midiChannel, NEvents, type, baseNote, notesPlayer, musicManager),
  _synthRange(SYNTH_OCTAVES, 7, 127) {
  setTypeSpecificColor();
}

void MonoSequence::setTypeSpecificColor(){
    setColor(MONO_SYNTH_COLOR_IDX);
}

// Hysteretic mapping of the Left axis onto a slot (arp-tone x octave index).
// Requires the reading to move a margin into the next slot before switching,
// which tames boundary flutter while sweeping.
uint8_t MonoSequence::_computeSlot() {
    uint8_t idx = _currentChordType & 0x0F;
    if (idx >= N_CHORDS) idx = 0;

    uint8_t arpCount = 1;
    for (uint8_t i = 1; i < N_STEPS; i++) {
        if (arpChordStep[idx][i] == 0) break;
        arpCount++;
    }
    uint8_t totalSlots = arpCount * SYNTH_OCTAVES;
    if (totalSlots < 1) totalSlots = 1;

    int slotWidth = 128 / totalSlots;
    if (slotWidth < 1) slotWidth = 1;
    int reading = constrain(_liveRangeReading, 0, 127);
    int raw = reading / slotWidth;
    if (raw >= totalSlots) raw = totalSlots - 1;

    // Hysteresis: only leave the current slot once we're a margin into the next
    if (_currentSlot >= 0 && _currentSlot < totalSlots && raw != _currentSlot) {
        int into = reading - raw * slotWidth;   // 0..slotWidth within candidate
        int margin = slotWidth / 3;
        if (raw > _currentSlot) { if (into < margin) raw = _currentSlot; }
        else                    { if (into > slotWidth - margin) raw = _currentSlot; }
    }
    _currentSlot = raw;
    return (uint8_t)raw;
}

void MonoSequence::_silence() {
    if (!_sounding) return;
    uint8_t ch = getMidiChannel();
    usbMIDI.sendNoteOff(_currentNote, 0, ch);
    Serial1.write(0x80 | ((ch - 1) & 0x0F));
    Serial1.write(_currentNote & 0x7F);
    Serial1.write(0x00);
    _sounding = false;
}

// Recompute the live note from the latched chord (root + type) and the current
// Left-axis position, then play it with LEGATO (new note-on before old note-off)
// so sweeping glides instead of re-attacking. Gated by axis presence.
void MonoSequence::_retriggerLiveNote() {
    if (!_chordLatched) return;

    // Presence gate: sound only while Left OR Center is present
    if (!_leftPresent && !_centerPresent) { _silence(); return; }

    uint8_t ch = getMidiChannel();
    uint8_t idx = _currentChordType & 0x0F;
    if (idx >= N_CHORDS) idx = 0;

    uint8_t arpCount = 1;
    for (uint8_t i = 1; i < N_STEPS; i++) {
        if (arpChordStep[idx][i] == 0) break;
        arpCount++;
    }

    uint8_t slot = _computeSlot();
    uint8_t zone = slot / arpCount;
    uint8_t step = slot % arpCount;

    int pitchClass = _currentRootNote % 12;
    int interval   = arpChordStep[idx][step];
    int note = BASE_NOTE + pitchClass + interval + zone * 12;
    note = constrain(note, 0, 127);
    uint8_t targetNote = (uint8_t)note;

    // No change: keep current note ringing (avoid spam while sweeping)
    if (_sounding && targetNote == _currentNote) return;

    uint8_t oldNote = _currentNote;
    bool wasSounding = _sounding;

    // Legato: start the new note first
    usbMIDI.sendNoteOn(targetNote, _liveVelocity, ch);
    Serial1.write(0x90 | ((ch - 1) & 0x0F));
    Serial1.write(targetNote & 0x7F);
    Serial1.write(_liveVelocity & 0x7F);

    // ...then release the previous one (overlap = glide, no envelope re-attack)
    if (wasSounding && oldNote != targetNote) {
        usbMIDI.sendNoteOff(oldNote, 0, ch);
        Serial1.write(0x80 | ((ch - 1) & 0x0F));
        Serial1.write(oldNote & 0x7F);
        Serial1.write(0x00);
    }

    _currentNote = targetNote;
    _sounding = true;
}

void MonoSequence::playLiveNoteOn(uint8_t rootNote, uint8_t velocity, uint8_t chordType) {
    (void)velocity;
    // Snapshot the chord: this root press LATCHES root + type until the next press
    _currentRootNote = rootNote;
    _currentChordType = chordType;
    _chordLatched = true;
    _currentSlot = -1;     // Re-evaluate slot for the new chord
    _tickCount = 0;        // Reset roll phase on new chord
    _retriggerLiveNote();
}

void MonoSequence::playLiveNoteOff(uint8_t rootNote, uint8_t chordType) {
    (void)rootNote;
    (void)chordType;
    // Hard stop + unlatch. NOTE: the chord is latched, so the manager does NOT
    // call this on pad release; this is for transport stop / mode exit / panic.
    _silence();
    _chordLatched = false;
    _rollActive = false;
}

void MonoSequence::handleLiveThreeAxis(ControlCommand command) {
    if (command.controlType != THREE_AXIS) return;
    switch (command.commandType) {
        case CHANGE_LEFT:
            _leftPresent = true;
            _liveRangeReading = command.value;
            // Re-fire immediately so the axis modulates the latched note in real time
            _retriggerLiveNote();
            break;
        case AXIS_ABSENT_LEFT:
            _leftPresent = false;
            if (!_centerPresent) _silence();   // No presence at all -> go quiet
            break;
        case CHANGE_CENTER:
            _centerPresent = true;
            // Roll speed - consistent with drum: higher value = slower roll
            if (command.value >= 125) {
                _rollActive = false;
            } else {
                _rollActive = true;
                // Inverted: 0-10 = fastest 32nd (1), 120 = slowest half note (16)
                // _rollDivision values: 1=32nd, 2=16th, 4=8th, 8=quarter, 16=half
                _rollDivision = ::map(constrain(command.value, 0, 120), 0, 120, 1, 16);
            }
            // Center presence alone can voice the latched chord
            if (_chordLatched && !_sounding) _retriggerLiveNote();
            break;
        case AXIS_ABSENT_CENTER:
            _centerPresent = false;
            _rollActive = false;
            if (!_leftPresent) _silence();     // No presence at all -> go quiet
            break;
        case CHANGE_RIGHT:
            _liveVelocity = command.value;
            break;
        default:
            break;
    }
}

void MonoSequence::handleLiveSequencerTick() {
    // 16th note tick - not used for rolls (they're on half-ticks)
}

void MonoSequence::handleLiveHalfTick() {
    _tickCount++;

    // Roll mode on 32nd-note resolution. Hard re-trigger (off then on) is the
    // point of a roll. Only while a latched note is actually sounding.
    if (_rollActive && _sounding && _rollDivision > 0) {
        if ((_tickCount % _rollDivision) == 0) {
            uint8_t ch = getMidiChannel();
            usbMIDI.sendNoteOff(_currentNote, 0, ch);
            Serial1.write(0x80 | ((ch - 1) & 0x0F));
            Serial1.write(_currentNote & 0x7F);
            Serial1.write(0x00);

            usbMIDI.sendNoteOn(_currentNote, _liveVelocity, ch);
            Serial1.write(0x90 | ((ch - 1) & 0x0F));
            Serial1.write(_currentNote & 0x7F);
            Serial1.write(_liveVelocity & 0x7F);
        }
    }
}

uint8_t MonoSequence::getLiveVelocity() const {
    return _liveVelocity;
}

void MonoSequence::playCurrentEventNote(){
    // Mute sequence playback during recording - only monitor input
    if(isRecording()) return;
    
    pointIterator(_currentPosition);
    it->setMidiChannel(getMidiChannel());
    if(isCurrentSequenceEnabled() && it->eventState()){
        if(it->isLiteralPitch()){
            _notesPlayer.queueNote(*it);
        } else {
            _musicManager.setCurrentSteps(it->getEventRead(), MONO_SYNTH);
            it->setEventNote(_musicManager.getCurrentChordNote());
            _notesPlayer.queueNote(*it);
        }
    }
}

void MonoSequence::editNoteInCurrentPosition(ControlCommand command){
  if(command.controlType == THREE_AXIS){ 
    switch(command.commandType){
      case CHANGE_LEFT:{
          pointIterator(_currentPosition);
          it->setEventRead(command.value);
          return;
      }
      case CHANGE_RIGHT:{
        pointIterator(_currentPosition);
        it->setEventVelocity(command.value);
        return;
      }
      case CHANGE_CENTER:{
          pointIterator(_currentPosition);
          it->setLength(constrain(remap(command.value, 0, 127, 1, 32),0,16));
          return;
      }
    } 
  }
  if(command.controlType == MIDI_NOTE){
    pointIterator(_currentPosition);
    it->setEventState(true);
    it->setEventNote(command.commandType);
    it->setEventVelocity(command.value);
    return;
  }
}