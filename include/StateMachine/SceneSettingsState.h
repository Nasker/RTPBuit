#include "BuitStateMachine.h"

class SceneSettingsState : public BuitState{
  BuitStateMachine& _buitMachine;
  int8_t _focusedPad = -1;  // -1 = no focus, 0-4 = pad focused awaiting confirm
public:
  SceneSettingsState (BuitStateMachine& voidMachine, BuitDevicesManager& devices);
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
