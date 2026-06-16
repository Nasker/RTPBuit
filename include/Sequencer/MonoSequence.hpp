#pragma once

#include "RTPEventNoteSequence.h"
#include "RTPTypeColors.h"
#include "RTPSmartRange.h"

class MonoSequence : public RTPEventNoteSequence{
public:
  MonoSequence(int midiChannel, int NEvents, int type, int baseNote, NotesPlayer& notesPlayer, MusicManager& musicManager);
  void setTypeSpecificColor() override;
  void playCurrentEventNote() override;
  void playLiveNoteOn(uint8_t rootNote, uint8_t velocity, uint8_t chordType) override;
  void playLiveNoteOff(uint8_t rootNote, uint8_t chordType) override;
  void handleLiveThreeAxis(ControlCommand command) override;
  void handleLiveSequencerTick() override;
  void handleLiveHalfTick() override;  // 32nd note resolution
  uint8_t getLiveVelocity() const override;
  void editNoteInCurrentPosition(ControlCommand command) override;
private:
  void _retriggerLiveNote();        // Recompute + (legato) play note from current axis/chord
  void _silence();                  // Stop the sounding note (keeps chord latched)
  uint8_t _computeSlot();           // Hysteretic Left-axis -> slot mapping

  uint8_t _liveVelocity = 90;
  uint8_t _liveRangeReading = 0;    // Left axis position (0-127), 0 = root note
  uint8_t _currentNote = 0;         // Currently playing note
  uint8_t _currentRootNote = 0;     // Root note of latched chord
  uint8_t _currentChordType = 0;    // Latched chord type
  bool _chordLatched = false;       // True once a chord is latched (persists after pad release)
  bool _sounding = false;           // True while a note is currently ringing

  // Axis presence (gate: sound only while Left OR Center is present)
  bool _leftPresent = false;
  bool _centerPresent = false;

  // Hysteresis for Left-axis slot selection
  int _currentSlot = -1;

  // Roll state (Center axis)
  bool _rollActive = false;
  uint8_t _rollDivision = 1;        // 1=32nd, 2=16th, 4=8th, 8=quarter, 16=half
  uint32_t _tickCount = 0;

  RTPSmartRange _synthRange;
};