#include "RTPEventNoteSequence.h"
#include "RTPTypeColors.h"

class HarmonySequence : public RTPEventNoteSequence{
public:
  HarmonySequence(int midiChannel, int NEvents, int type, int baseNote, NotesPlayer& notesPlayer, MusicManager& musicManager);
  void setTypeSpecificColor() override;
  void playCurrentEventNote() override;
  void editNoteInCurrentPosition(ControlCommand command) override;
  void playLiveNoteOn(uint8_t rootNote, uint8_t velocity, uint8_t chordType) override;
};

