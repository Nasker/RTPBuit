#include "BuitStateMachine.h"
#include "BuitFunctions/ChordionKeys.hpp"

class SequencePianoRollState : public BuitState{
  BuitStateMachine& _buitMachine;

  // --- live play state ---
  ChordionKeys _chordionKeys;

  // Poly / Mono / Bass
  uint8_t _currentVelocity = 90;
  uint8_t _currentSpread   = 4;
  uint8_t _currentOctave   = 0;
  int     _eventRead       = 64;

  // Drum theremin roll
  bool     _drumRollActive = false;
  uint8_t  _drumRollNote   = 36;
  uint8_t  _rollDivision   = 1;   // step interval: 1=16th 2=8th 4=quarter
  uint8_t  _rollVelocity   = 90;
  uint32_t _tickCount      = 0;

  // helpers
  void _paintTrellis();
  void _playLiveNote(uint8_t rootNote, uint8_t midiCh, uint8_t seqType);
  void _fireDrumHit(uint8_t note, uint8_t vel, uint8_t midiCh);

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
