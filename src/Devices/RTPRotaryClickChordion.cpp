#include "RTPRotaryClickChordion.hpp"
#include "RTPMainUnit.hpp"
#include "constants.h"
#include "ControlCommand.h"

void RTPRotaryClickDev::callbackFromRotary(RTPMainUnit* mainClass){
	long newPosition = read()/4;
        ControlCommand callbackCommand;
        callbackCommand.controlType = ROTARY;
  	if (newPosition < _oldPosition) {
   	        _oldPosition = newPosition;
                callbackCommand.value = newPosition;
                callbackCommand.commandType = ROTATING_LEFT;
                mainClass->actOnControlsCallback(callbackCommand);
  	}
  	else if (newPosition > _oldPosition) {
   		_oldPosition = newPosition;
                callbackCommand.value = newPosition;
                callbackCommand.commandType = ROTATING_RIGHT;
                mainClass->actOnControlsCallback(callbackCommand);
  	}
}

void RTPRotaryClickDev::callbackFromClicks(RTPMainUnit* mainClass){
        Update();
        ControlCommand callbackCommand;
        callbackCommand.controlType = PUSH_BUTTON;
        callbackCommand.value = 0;
        switch(clicks){
                case 1: callbackCommand.commandType = SINGLE_CLICK;
                        mainClass->actOnControlsCallback(callbackCommand);
                        break;

                case 2: callbackCommand.commandType = DOUBLE_CLICK;
                        mainClass->actOnControlsCallback(callbackCommand);
                        break;

                case 3: callbackCommand.commandType = TRIPLE_CLICK;
                        mainClass->actOnControlsCallback(callbackCommand);
                        break;

                case -1: callbackCommand.commandType = LONG_CLICK;
                        mainClass->actOnControlsCallback(callbackCommand);
                        break;

                default:  break;
  }      
}
