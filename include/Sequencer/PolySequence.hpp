#pragma once

#include "RTPEventNoteSequence.h"
#include "RTPTypeColors.h"

class PolySequence : public RTPEventNoteSequence{
public:
  PolySequence(int midiChannel, int NEvents, int type, int baseNote, NotesPlayer& notesPlayer, MusicManager& musicManager);
  void setTypeSpecificColor() override;
  void playCurrentEventNote() override;
  void editNoteInCurrentPosition(ControlCommand command) override;
};