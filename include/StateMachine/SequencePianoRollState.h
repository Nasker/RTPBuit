#include "BuitStateMachine.h"
#include "BuitFunctions/ChordionKeys.hpp"

class SequencePianoRollState : public BuitState{
  BuitStateMachine& _buitMachine;

  // --- live play state ---
  ChordionKeys _chordionKeys;

  // Drum theremin roll
  bool     _drumRollActive = false;
  uint8_t  _drumRollNote   = 36;
  uint8_t  _rollDivision   = 1;   // step interval: 1=16th 2=8th 4=quarter
  uint32_t _tickCount      = 0;

  // Track chord type active when each note was pressed (so note-off matches)
  uint8_t _activeChordTypes[128] = {0};

  // helpers
  void _paintTrellis();

public:
  SequencePianoRollState (BuitStateMachine& voidMachine, BuitDevicesManager& devices);
  void onEnter() override;
  void singleClick();
  void doubleClick();
  void tripleClick();
  void longClick();
  void rotaryTurned(ControlCommand command);
  void threeAxisChanged(ControlCommand command);
  void trellisPressed(ControlCommand command);
  void trellisReleased(ControlCommand command);
  void sequencerCallback(ControlCommand command);
  void midiNote(ControlCommand command);
  void midiNoteOff(ControlCommand command);
  void midiCC(ControlCommand command);
};
