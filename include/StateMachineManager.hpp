#pragma once

#include "BuitStateMachine.h"
#include "BuitDevicesManager.hpp"
#include "ControlCommand.h"
#include "constants.h"

class StateMachineManager{
    BuitStateMachine buitStateMachine;
    BuitDevicesManager& _devices;
 public:
    StateMachineManager(BuitDevicesManager& devices);
    ~StateMachineManager();
    void setInitialState();
    void handleActions(ControlCommand callbackControlCommand);
};
