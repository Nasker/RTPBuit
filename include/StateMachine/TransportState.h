#include "BuitStateMachine.h"

class TransportState : public BuitState{
  BuitStateMachine& _buitMachine;
public:
  TransportState(BuitStateMachine& buitMachine, BuitDevicesManager& devices);
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
  void midiCC(ControlCommand command);
};
