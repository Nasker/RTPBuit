/*
    Copyright 2021 Oscar Martínez
*/
#include "StateMachineManager.hpp"

StateMachineManager::StateMachineManager(BuitDevicesManager& devices):_devices(devices), buitStateMachine(devices){}

StateMachineManager::~StateMachineManager() {}

void StateMachineManager::setInitialState() {
  buitStateMachine.setState(buitStateMachine.getInitState());
}

void StateMachineManager::handleActions(ControlCommand callbackControlCommand) {
  switch(callbackControlCommand.controlType){
    case PUSH_BUTTON:
      switch (callbackControlCommand.commandType) {
          case SINGLE_CLICK:
            buitStateMachine.singleClick();
            break;
          case DOUBLE_CLICK:
            buitStateMachine.doubleClick();
            break;
          case LONG_CLICK:
            buitStateMachine.longClick();
            break;
      }
    case ROTARY:
      buitStateMachine.rotaryTurned(callbackControlCommand);
      break;
    case THREE_AXIS: 
      buitStateMachine.threeAxisChanged(callbackControlCommand);
      break;
    case TRELLIS:
      switch (callbackControlCommand.commandType) {
        case PRESSED:
            buitStateMachine.trellisPressed(callbackControlCommand);
            break;
        case RELEASED:    
            //buitStateMachine.trellisReleased(callbackControlCommand);
            break;
        default:
            break;
        }
      break;
    case SEQUENCER:
      buitStateMachine.sequencerCallback(callbackControlCommand);
      break;
  } 
}