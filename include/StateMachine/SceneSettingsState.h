#include "BuitStateMachine.h"

class SceneSettingsState : public BuitState{
  BuitStateMachine& _buitMachine;
public:
  SceneSettingsState (BuitStateMachine& voidMachine, BuitDevicesManager& devices);
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
