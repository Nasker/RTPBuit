#include "BuitStateMachine.h"

class SequencePianoRollState : public BuitState{
  BuitStateMachine& _buitMachine;
public:
  SequencePianoRollState (BuitStateMachine& voidMachine, BuitDevicesManager& devices);
  void singleClick();
  void doubleClick();
  void longClick();
  void rotaryTurned(ControlCommand command);
  void threeAxisChanged(ControlCommand command);
  void trellisPressed(ControlCommand command);
  void trellisReleased(ControlCommand command);
  void sequencerCallback(ControlCommand command);
};
