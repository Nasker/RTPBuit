/*
    Copyright 2021 Oscar Martínez
*/
#include "StateMachineManager.hpp"

StateMachineManager::StateMachineManager(BuitDevicesManager& devices):_devices(devices), buitStateMachine(devices){}

StateMachineManager::~StateMachineManager() {}

void StateMachineManager::setInitialState() {
  buitStateMachine.setState(buitStateMachine.getSceneEditState());
}

void StateMachineManager::handleActions(ControlCommand callbackControlCommand) {
  switch(callbackControlCommand.controlType){
    case PUSH_BUTTON:
      switch (callbackControlCommand.commandType) {
          case SINGLE_CLICK:
            buitStateMachine.singleClick();
            return;
          case DOUBLE_CLICK:
            buitStateMachine.doubleClick();
            return;
          case LONG_CLICK:
            buitStateMachine.longClick();
            return;
      }
    case ROTARY:
      buitStateMachine.rotaryTurned(callbackControlCommand);
      return;
    case THREE_AXIS: 
      buitStateMachine.threeAxisChanged(callbackControlCommand);
      return;
    case TRELLIS:
      switch (callbackControlCommand.commandType) {
        case PRESSED:
            buitStateMachine.trellisPressed(callbackControlCommand);
            return;
        case RELEASED:    
            //buitStateMachine.trellisReleased(callbackControlCommand);
            return;
        default:
            return;
        }
      break;
    case SEQUENCER:
      buitStateMachine.sequencerCallback(callbackControlCommand);
      return;
  } 
}