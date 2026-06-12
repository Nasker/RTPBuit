#include "Arduino.h"
#include "SequencePianoRollState.h"
#include "constants.h"

// Drum theremin: presence threshold (sensor values <= this = no hand)
static constexpr int DRUM_PRESENCE_THRESHOLD = 2;
// Number of drum zones mapped across the left-axis range (0-127)
static constexpr uint8_t DRUM_ZONES = 16;

// Note name lookup (chromatic, starting at C)
static const char* NOTE_NAMES[12] = {
  "C","C#","D","D#","E","F","F#","G","G#","A","A#","B"
};
// Chord type names for the 16 chordionArray bitmask values (bits 0-3)
static const char* CHORD_TYPE_NAMES[16] = {
  "note","maj","min","maj7","min7","dom7","sus2","sus4",
  "aug","dim","add9","maj9","min9","11th","13th","full"
};

SequencePianoRollState::SequencePianoRollState(BuitStateMachine& buitMachine, BuitDevicesManager& devices) : BuitState(devices), _buitMachine(buitMachine) {
  Serial.println("SequencePianoRollState");
  _buitMachine = buitMachine;
}

// ---------------------------------------------------------------------------
// Paint trellis for live play mode
// ---------------------------------------------------------------------------
void SequencePianoRollState::_paintTrellis() {
  uint8_t seqType = _devices.getSelectedSequenceType();
  uint32_t seqColor = (uint32_t)_devices.getSelectedSequenceColor();

  _devices.clearTrellis();

  // Black key pad indices in the 12-note chromatic layout (C=0): C#,D#,F#,G#,A#
  static const bool isBlackKey[12] = {false,true,false,true,false,false,true,false,true,false,true,false};

  if (seqType == DRUM_PART) {
    // All 16 pads = drum notes in sequence colour
    for (uint8_t i = 0; i < 16; i++)
      _devices.setTrellisButtonColor(i, seqColor);
  } else {
    // Pads 0-11: note keys — black keys dimmer to simulate piano layout
    for (uint8_t i = 0; i < 12; i++) {
      uint32_t dimmed = (((seqColor >> 16 & 0xFF) * 35 / 255) << 16)
                      | (((seqColor >>  8 & 0xFF) * 35 / 255) <<  8)
                      |  ((seqColor       & 0xFF) * 35 / 255);
      uint32_t c = isBlackKey[i] ? dimmed : seqColor;
      _devices.setTrellisButtonColor(i, c);
    }
    // Pads 12-15: chord modifier toggles (dim white = off)
    for (uint8_t i = 0; i < N_CHORDION_KEYS; i++)
      _devices.setTrellisButtonColor(12 + i, 0x101010);
  }
  _devices.showTrellis();
}

// ---------------------------------------------------------------------------
// onEnter
// ---------------------------------------------------------------------------
void SequencePianoRollState::onEnter() {
  _chordionKeys.initSetup();
  _drumRollActive = false;
  _tickCount = 0;
  _paintTrellis();

  uint8_t seqType = _devices.getSelectedSequenceType();
  String typeName;
  switch (seqType) {
    case DRUM_PART:  typeName = "Drum";  break;
    case BASS_SYNTH: typeName = "Bass";  break;
    case MONO_SYNTH: typeName = "Lead";  break;
    case POLY_SYNTH: typeName = "Poly";  break;
    default:         typeName = "Live";  break;
  }
  _devices.printToScreen("Piano Roll", typeName, "Live");
}

// ---------------------------------------------------------------------------
// Private: fire a single drum hit (note-on + note-off) and optionally record
// ---------------------------------------------------------------------------
void SequencePianoRollState::_fireDrumHit(uint8_t note, uint8_t vel, uint8_t midiCh) {
  usbMIDI.sendNoteOn(note, vel, midiCh);
  Serial1.write(0x90 | ((midiCh - 1) & 0x0F));
  Serial1.write(note & 0x7F);
  Serial1.write(vel & 0x7F);
  usbMIDI.sendNoteOff(note, 0, midiCh);
  Serial1.write(0x80 | ((midiCh - 1) & 0x0F));
  Serial1.write(note & 0x7F);
  Serial1.write(0x00);
  if (_devices.isSelectedSequenceRecording()) {
    _devices.recorderNoteOn(note, vel);
    _devices.recorderNoteOff(note);
  }
}

// ---------------------------------------------------------------------------
// Private: play live note(s) depending on sequence type
// ---------------------------------------------------------------------------
void SequencePianoRollState::_playLiveNote(uint8_t rootNote, uint8_t midiCh, uint8_t seqType) {
  switch (seqType) {
    case POLY_SYNTH: {
      queue<int> notes = _devices.getMusicManager().getAutoharpChordNotes(_eventRead, _currentSpread);
      while (!notes.empty()) {
        uint8_t n = (uint8_t)notes.front();
        usbMIDI.sendNoteOn(n, _currentVelocity, midiCh);
        Serial1.write(0x90 | ((midiCh - 1) & 0x0F));
        Serial1.write(n & 0x7F);
        Serial1.write(_currentVelocity & 0x7F);
        notes.pop();
      }
      break;
    }
    case MONO_SYNTH:
    case BASS_SYNTH: {
      _devices.getMusicManager().setCurrentSteps(rootNote, seqType);
      uint8_t note = (uint8_t)_devices.getMusicManager().getCurrentChordNote();
      usbMIDI.sendNoteOn(note, _currentVelocity, midiCh);
      Serial1.write(0x90 | ((midiCh - 1) & 0x0F));
      Serial1.write(note & 0x7F);
      Serial1.write(_currentVelocity & 0x7F);
      break;
    }
    default:
      break;
  }
}

// ---------------------------------------------------------------------------
// singleClick — no action
// ---------------------------------------------------------------------------
void SequencePianoRollState::singleClick() {}

// ---------------------------------------------------------------------------
// doubleClick — back to sequence editor
// ---------------------------------------------------------------------------
void SequencePianoRollState::doubleClick() {
  _buitMachine.setState(_buitMachine.getSequenceEditState());
}

// ---------------------------------------------------------------------------
// tripleClick — toggle recording on/off
// ---------------------------------------------------------------------------
void SequencePianoRollState::tripleClick() {
  _devices.toggleSelectedSequenceRecording();
}

void SequencePianoRollState::longClick() {}

// ---------------------------------------------------------------------------
// rotaryTurned — nudge page (useful while recording)
// ---------------------------------------------------------------------------
void SequencePianoRollState::rotaryTurned(ControlCommand command) {
  //_devices.nudgePage(command);
}

// ---------------------------------------------------------------------------
// threeAxisChanged
// ---------------------------------------------------------------------------
void SequencePianoRollState::threeAxisChanged(ControlCommand command) {
  uint8_t seqType = _devices.getSelectedSequenceType();
  int val = command.value;

  if (seqType == DRUM_PART) {
    switch (command.commandType) {
      case CHANGE_LEFT:
        if (val <= DRUM_PRESENCE_THRESHOLD) {
          _drumRollActive = false;
        } else {
          _drumRollActive = true;
          // Map 0-127 → 16 zones
          uint8_t zone = (uint8_t)constrain(::map(val, 0, 127, 0, DRUM_ZONES - 1), 0, DRUM_ZONES - 1);
          _drumRollNote = BASE_NOTE + zone;
        }
        break;
      case CHANGE_CENTER:
        // Map 0-127 → subdivision step interval {4, 2, 1} (quarter / 8th / 16th)
        if (val < 43)       _rollDivision = 4;
        else if (val < 85)  _rollDivision = 2;
        else                _rollDivision = 1;
        break;
      case CHANGE_RIGHT:
        _rollVelocity = (uint8_t)constrain(::map(val, 0, 127, 1, 127), 1, 127);
        break;
    }
  } else {
    // Poly / Mono / Bass
    switch (command.commandType) {
      case CHANGE_LEFT:
        if (seqType == POLY_SYNTH)
          _eventRead = val;
        else
          _currentOctave = (uint8_t)constrain(::map(val, 0, 127, 0, 4), 0, 4);
        break;
      case CHANGE_CENTER:
        // dual-purpose: spread (poly) or length, also acts as velocity envelope
        if (seqType == POLY_SYNTH)
          _currentSpread = (uint8_t)constrain(::map(val, 0, 127, 1, 8), 1, 8);
        break;
      case CHANGE_RIGHT:
        _currentVelocity = (uint8_t)constrain(::map(val, 0, 127, 1, 127), 1, 127);
        break;
    }
  }
}

// ---------------------------------------------------------------------------
// trellisPressed
// ---------------------------------------------------------------------------
void SequencePianoRollState::trellisPressed(ControlCommand command) {
  uint8_t pad = (uint8_t)command.value;
  uint8_t seqType = _devices.getSelectedSequenceType();
  uint8_t midiCh = (uint8_t)_devices.getSelectedSequenceMidichannel();

  if (seqType == DRUM_PART) {
    uint8_t note = BASE_NOTE + pad;
    usbMIDI.sendNoteOn(note, _currentVelocity, midiCh);
    Serial1.write(0x90 | ((midiCh - 1) & 0x0F));
    Serial1.write(note & 0x7F);
    Serial1.write(_currentVelocity & 0x7F);
    if (_devices.isSelectedSequenceRecording())
      _devices.recorderNoteOn(note, _currentVelocity);
    _devices.setTrellisButtonColor(pad, 0xFFFFFF);
    _devices.showTrellis();
  } else {
    if (pad < 12) {
      // Note key
      uint8_t rootNote = BASE_NOTE + pad;
      _playLiveNote(rootNote, midiCh, seqType);
      if (_devices.isSelectedSequenceRecording())
        _devices.recorderNoteOn(rootNote, _currentVelocity);
      _devices.setTrellisButtonColor(pad, 0xFFFFFF);
      _devices.showTrellis();
      // Show root note + chord type on OLED bottom line
      {
        uint8_t noteName = pad % 12;
        uint8_t chordType = _chordionKeys.getChordType();
        String chordStr = String(NOTE_NAMES[noteName]) + " " + String(CHORD_TYPE_NAMES[chordType & 0x0F]);
        _devices.printToScreen("Piano Roll", _devices.getSequencer().getSelectedSequenceTypeName(), chordStr);
      }
    } else {
      // Chord modifier momentary (held = on, released = off)
      uint8_t modIdx = pad - 12;
      _chordionKeys.enableChordionKey(modIdx);
      _devices.setTrellisButtonColor(pad, 0xFFFFFF);
      _devices.showTrellis();
      // Preview chord type on OLED
      String preview = String(CHORD_TYPE_NAMES[_chordionKeys.getChordType() & 0x0F]);
      _devices.printToScreen("Piano Roll", _devices.getSequencer().getSelectedSequenceTypeName(), "[ " + preview + " ]");
    }
  }
}

// ---------------------------------------------------------------------------
// trellisReleased
// ---------------------------------------------------------------------------
void SequencePianoRollState::trellisReleased(ControlCommand command) {
  uint8_t pad = (uint8_t)command.value;
  uint8_t seqType = _devices.getSelectedSequenceType();
  uint8_t midiCh = (uint8_t)_devices.getSelectedSequenceMidichannel();

  if (seqType == DRUM_PART) {
    uint8_t note = BASE_NOTE + pad;
    usbMIDI.sendNoteOff(note, 0, midiCh);
    Serial1.write(0x80 | ((midiCh - 1) & 0x0F));
    Serial1.write(note & 0x7F);
    Serial1.write(0x00);
    if (_devices.isSelectedSequenceRecording())
      _devices.recorderNoteOff(note);
    _devices.setTrellisButtonColor(pad, (uint32_t)_devices.getSelectedSequenceColor());
    _devices.showTrellis();
  } else if (pad < 12) {
    uint8_t rootNote = BASE_NOTE + pad;
    if (seqType == POLY_SYNTH) {
      // Send note-offs for all chord notes that were started
      queue<int> notes = _devices.getMusicManager().getAutoharpChordNotes(_eventRead, _currentSpread);
      while (!notes.empty()) {
        uint8_t n = (uint8_t)notes.front();
        usbMIDI.sendNoteOff(n, 0, midiCh);
        Serial1.write(0x80 | ((midiCh - 1) & 0x0F));
        Serial1.write(n & 0x7F);
        Serial1.write(0x00);
        notes.pop();
      }
    } else {
      // Mono/bass: single note off
      _devices.getMusicManager().setCurrentSteps(rootNote, seqType);
      uint8_t note = (uint8_t)_devices.getMusicManager().getCurrentChordNote();
      usbMIDI.sendNoteOff(note, 0, midiCh);
      Serial1.write(0x80 | ((midiCh - 1) & 0x0F));
      Serial1.write(note & 0x7F);
      Serial1.write(0x00);
    }
    if (_devices.isSelectedSequenceRecording())
      _devices.recorderNoteOff(rootNote);
    {
      static const bool isBlackKey[12] = {false,true,false,true,false,false,true,false,true,false,true,false};
      uint32_t sc = (uint32_t)_devices.getSelectedSequenceColor();
      uint32_t dimmed = (((sc >> 16 & 0xFF) * 35 / 255) << 16)
                      | (((sc >>  8 & 0xFF) * 35 / 255) <<  8)
                      |  ((sc       & 0xFF) * 35 / 255);
      _devices.setTrellisButtonColor(pad, (pad < 12 && isBlackKey[pad]) ? dimmed : sc);
    }
    _devices.showTrellis();
  }
  // Modifier pad release (12-15): disable key (momentary), restore LED, update OLED preview
  if (pad >= 12 && pad <= 15) {
    _chordionKeys.disableChordionKey(pad - 12);
    _devices.setTrellisButtonColor(pad, 0x101010);
    _devices.showTrellis();
    String preview = String(CHORD_TYPE_NAMES[_chordionKeys.getChordType() & 0x0F]);
    _devices.printToScreen("Piano Roll", _devices.getSequencer().getSelectedSequenceTypeName(), "[ " + preview + " ]");
  }
}

// ---------------------------------------------------------------------------
// sequencerCallback — advance tick counter + drum theremin roll
// ---------------------------------------------------------------------------
void SequencePianoRollState::sequencerCallback(ControlCommand command) {
  if (command.commandType != GRID_TICK) return;

  _tickCount++;

  uint8_t seqType = _devices.getSelectedSequenceType();
  uint8_t midiCh  = (uint8_t)_devices.getSelectedSequenceMidichannel();

  if (seqType == DRUM_PART && _drumRollActive) {
    if ((_tickCount % _rollDivision) == 0)
      _fireDrumHit(_drumRollNote, _rollVelocity, midiCh);
  }

  if (_devices.isSelectedSequenceRecording())
    _devices.displayCursorInSequence(command);
}

void SequencePianoRollState::midiNote(ControlCommand command) {}

void SequencePianoRollState::midiNoteOff(ControlCommand command) {}

void SequencePianoRollState::midiCC(ControlCommand command) {}