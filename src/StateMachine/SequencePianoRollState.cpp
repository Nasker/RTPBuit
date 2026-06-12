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
  for (int i = 0; i < 128; i++) _activeChordTypes[i] = 0;
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
          uint8_t zone = (uint8_t)constrain(::map(val, 0, 127, 0, DRUM_ZONES - 1), 0, DRUM_ZONES - 1);
          _drumRollNote = BASE_NOTE + zone;
        }
        break;
      case CHANGE_CENTER:
        if (val < 43)       _rollDivision = 4;
        else if (val < 85)  _rollDivision = 2;
        else                _rollDivision = 1;
        break;
      case CHANGE_RIGHT:
        _devices.handleLiveThreeAxis(command);
        break;
    }
  } else {
    _devices.handleLiveThreeAxis(command);
  }
}

// ---------------------------------------------------------------------------
// trellisPressed
// ---------------------------------------------------------------------------
void SequencePianoRollState::trellisPressed(ControlCommand command) {
  uint8_t pad = (uint8_t)command.value;
  uint8_t seqType = _devices.getSelectedSequenceType();

  uint8_t liveVel = _devices.getLiveVelocity();
  if (seqType == DRUM_PART) {
    uint8_t note = BASE_NOTE + pad;
    _devices.playLiveNoteOn(note, liveVel, 0);
    if (_devices.isSelectedSequenceRecording())
      _devices.recorderNoteOn(note, liveVel);
    _devices.setTrellisButtonColor(pad, 0xFFFFFF);
    _devices.showTrellis();
  } else {
    if (pad < 12) {
      uint8_t rootNote = BASE_NOTE + pad;
      uint8_t chordType = _chordionKeys.getChordType();
      _activeChordTypes[rootNote] = chordType;
      _devices.playLiveNoteOn(rootNote, liveVel, chordType);
      if (_devices.isSelectedSequenceRecording())
        _devices.recorderNoteOn(rootNote, liveVel);
      _devices.setTrellisButtonColor(pad, 0xFFFFFF);
      _devices.showTrellis();
      String chordStr = String(NOTE_NAMES[pad % 12]) + " " + String(CHORD_TYPE_NAMES[chordType & 0x0F]);
      _devices.printToScreen("Piano Roll", _devices.getSequencer().getSelectedSequenceTypeName(), chordStr);
    } else {
      uint8_t modIdx = pad - 12;
      _chordionKeys.enableChordionKey(modIdx);
      _devices.setTrellisButtonColor(pad, 0xFFFFFF);
      _devices.showTrellis();
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

  if (seqType == DRUM_PART) {
    uint8_t note = BASE_NOTE + pad;
    _devices.playLiveNoteOff(note, 0);
    if (_devices.isSelectedSequenceRecording())
      _devices.recorderNoteOff(note);
    _devices.setTrellisButtonColor(pad, (uint32_t)_devices.getSelectedSequenceColor());
    _devices.showTrellis();
  } else if (pad < 12) {
    uint8_t rootNote = BASE_NOTE + pad;
    uint8_t ct = _activeChordTypes[rootNote];
    _devices.playLiveNoteOff(rootNote, ct);
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

  if (seqType == DRUM_PART && _drumRollActive) {
    if ((_tickCount % _rollDivision) == 0)
      _devices.playLiveNoteOn(_drumRollNote, _devices.getLiveVelocity(), 0);
  }

  if (_devices.isSelectedSequenceRecording())
    _devices.displayCursorInSequence(command);
}

void SequencePianoRollState::midiNote(ControlCommand command) {}

void SequencePianoRollState::midiNoteOff(ControlCommand command) {}

void SequencePianoRollState::midiCC(ControlCommand command) {}