#pragma once

#include "RTPEventNoteSequence.h"
#include "RTPTypeColors.h"

class MonoSequence : public RTPEventNoteSequence{
public:
  MonoSequence(int midiChannel, int NEvents, int type, int baseNote, NotesPlayer& notesPlayer, MusicManager& musicManager);
  void setTypeSpecificColor() override;
  void playCurrentEventNote() override;
  void playLiveNoteOn(uint8_t rootNote, uint8_t velocity, uint8_t chordType) override;
  void playLiveNoteOff(uint8_t rootNote, uint8_t chordType) override;
  void handleLiveThreeAxis(ControlCommand command) override;
  uint8_t getLiveVelocity() const override;
  void editNoteInCurrentPosition(ControlCommand command) override;
private:
  uint8_t _liveVelocity = 90;
  uint8_t _liveOctave = 0;
  queue<uint8_t> _liveRingingNotes;
};