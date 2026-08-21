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
#include "PatternBankState.h"

#include "Arduino.h"

BuitStateMachine::BuitStateMachine(BuitDevicesManager& outDevices){ //, LiquidScreen& lScreen
	_initState             = std::make_unique<InitState>(*this, outDevices);
	_transportState        = std::make_unique<TransportState>(*this, outDevices);
	_globalSettingsState   = std::make_unique<GlobalSettingsState>(*this, outDevices);
	_sceneEditState        = std::make_unique<SceneEditState>(*this, outDevices);
	_sceneSettingsState    = std::make_unique<SceneSettingsState>(*this, outDevices);
	_sequenceEditState     = std::make_unique<SequenceEditState>(*this, outDevices);
	_sequencePianoRollState = std::make_unique<SequencePianoRollState>(*this, outDevices);
	_sequenceSelectState   = std::make_unique<SequenceSelectState>(*this, outDevices);
	_sequenceSettingsState = std::make_unique<SequenceSettingsState>(*this, outDevices);
	_patternBankLoadState  = std::make_unique<PatternBankState>(*this, outDevices, PatternBankMode::Load);
	_patternBankSaveState  = std::make_unique<PatternBankState>(*this, outDevices, PatternBankMode::Save);
	_state = _sceneEditState.get();
}

void BuitStateMachine::singleClick(){
  _state->singleClick();
}

void BuitStateMachine::doubleClick(){
  _state->doubleClick();
}

void BuitStateMachine::tripleClick(){
  _state->tripleClick();
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

void BuitStateMachine::midiNote(ControlCommand command){
  _state->midiNote(command);
}

void BuitStateMachine::midiNoteOff(ControlCommand command){
  _state->midiNoteOff(command);
}

void BuitStateMachine::midiCC(ControlCommand command){
  _state->midiCC(command);
}

void BuitStateMachine::setState(BuitState* state){
  _state = state;
  _state->onEnter();
}

BuitState* BuitStateMachine::getInitState(){
  return _initState.get();
}

BuitState* BuitStateMachine::getTransportState(){
  return _transportState.get();
}

BuitState* BuitStateMachine::getGlobalSettingsState(){
  return _globalSettingsState.get();
}

BuitState* BuitStateMachine::getSceneEditState(){
  return _sceneEditState.get();
}

BuitState* BuitStateMachine::getSceneSettingsState(){
  return _sceneSettingsState.get();
}

BuitState* BuitStateMachine::getSequenceEditState(){
  return _sequenceEditState.get();
}

BuitState* BuitStateMachine::getSequencePianoRollState(){
  return _sequencePianoRollState.get();
}

BuitState* BuitStateMachine::getSequenceSelectState(){
  return _sequenceSelectState.get();
}

BuitState* BuitStateMachine::getSequenceSettingsState(){
  return _sequenceSettingsState.get();
}

BuitState* BuitStateMachine::getPatternBankLoadState(){
  return _patternBankLoadState.get();
}

BuitState* BuitStateMachine::getPatternBankSaveState(){
  return _patternBankSaveState.get();
}