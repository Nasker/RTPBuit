#include "RTPEventNoteSequence.h"
#include "RTPTypeColors.h"

class HarmonySequence : public RTPEventNoteSequence{
public:
  HarmonySequence(int midiChannel, int NEvents, int type, int baseNote, NotesPlayer& notesPlayer, MusicManager& musicManager);
  void fordwardSequence() override;
  void setTypeSpecificColor() override;
  void playCurrentEventNote() override;
  void editNoteInCurrentPosition(ControlCommand command) override;
private:
  int _countTracker = 0;
  static const int N_COUNTS = 4;
};

