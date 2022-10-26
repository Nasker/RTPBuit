#include "BuitStateMachine.h"
#include "InitState.h"
#include "GlobalSettingsState.h"
#include "TransportState.h"
#include "SceneEditState.h"
#include "SceneSettingsState.h"
#include "SequenceEditState.h"
#include "SequencePianoRollState.h"
#include "SequenceSelectState.h"
#include "SequenceSettingsState.h"

#include "Arduino.h"

BuitStateMachine::BuitStateMachine(BuitDevicesManager& outDevices){ //, LiquidScreen& lScreen
  _state = new InitState(*this, outDevices);
	_initState = new InitState(*this, outDevices);
	_transportState = new TransportState(*this, outDevices);
	_globalSettingsState = new GlobalSettingsState(*this, outDevices);
	_sceneEditState = new SceneEditState(*this, outDevices);
	_sceneSettingsState = new SceneSettingsState(*this, outDevices);
	_sequenceEditState = new SequenceEditState(*this, outDevices);
	_sequencePianoRollState = new SequencePianoRollState(*this, outDevices);
	_sequenceSelectState = new SequenceSelectState(*this, outDevices);
	_sequenceSettingsState = new SequenceSettingsState(*this, outDevices);
}

void BuitStateMachine::singleClick(){
  _state->singleClick();
}

void BuitStateMachine::doubleClick(){
  _state->doubleClick();
}

void BuitStateMachine::longClick(){
  _state->longClick();
}

void BuitStateMachine::rotaryTurned(ControlCommand command){
  _state->rotaryTurned(command);
}

void BuitStateMachine::threeAxisChanged(ControlCommand command){
  _state->threeAxisChanged(command);
}

void BuitStateMachine::trellisPressed(ControlCommand command){
  _state->trellisPressed(command);
}

void BuitStateMachine::trellisReleased(ControlCommand command){
  _state->trellisReleased(command);
}

void BuitStateMachine::sequencerCallback(ControlCommand command){
  _state->sequencerCallback(command);
}

void BuitStateMachine::setState(BuitState* state){
  _state = state;
}

BuitState* BuitStateMachine::getInitState(){
  return _initState;
}

BuitState* BuitStateMachine::getTransportState(){
  return _transportState;
}

BuitState* BuitStateMachine::getGlobalSettingsState(){
  return _globalSettingsState;
}

BuitState* BuitStateMachine::getSceneEditState(){
  return _sceneEditState;
}

BuitState* BuitStateMachine::getSceneSettingsState(){
  return _sceneSettingsState;
}

BuitState* BuitStateMachine::getSequenceEditState(){
  return _sequenceEditState;
}

BuitState* BuitStateMachine::getSequencePianoRollState(){
  return _sequencePianoRollState;
}

BuitState* BuitStateMachine::getSequenceSelectState(){
  return _sequenceSelectState;
}

BuitState* BuitStateMachine::getSequenceSettingsState(){
  return _sequenceSettingsState;
}