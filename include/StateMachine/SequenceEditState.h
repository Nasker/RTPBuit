#include "BuitStateMachine.h"

class SequenceEditState : public BuitState{
  BuitStateMachine& _buitMachine;
public:
  SequenceEditState (BuitStateMachine& voidMachine, BuitDevicesManager& devices);
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
