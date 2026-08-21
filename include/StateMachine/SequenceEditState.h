#include "BuitStateMachine.h"

class MidiRouter;

class SequenceEditState : public BuitState{
  BuitStateMachine& _buitMachine;
public:
  SequenceEditState (BuitStateMachine& voidMachine, BuitDevicesManager& devices);
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
  static void setRouter(MidiRouter* router);
private:
  static MidiRouter* _router;
};
