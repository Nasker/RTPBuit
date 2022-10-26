#pragma once

#include "BuitDevicesManager.hpp"

class BuitState {
protected:
	BuitDevicesManager& _devices;
public:
	BuitState(BuitDevicesManager& devices) : _devices(devices) {}
  	virtual void singleClick() = 0;
	virtual void doubleClick() = 0;
	virtual void longClick() = 0;
	virtual void rotaryTurned(ControlCommand command) = 0;
	virtual void threeAxisChanged(ControlCommand command) = 0;
	virtual void trellisPressed(ControlCommand command) = 0;
	virtual void trellisReleased(ControlCommand command) = 0;
	virtual void sequencerCallback(ControlCommand command) = 0;
};
