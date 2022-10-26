#pragma once

#include "BuitState.h"
#include "BuitDevicesManager.hpp"

class BuitStateMachine{
	BuitState* _state;
	BuitState* _initState;
	BuitState* _transportState;
	BuitState* _globalSettingsState;
	BuitState* _sceneEditState;
	BuitState* _sceneSettingsState;
	BuitState* _sequenceEditState;
	BuitState* _sequencePianoRollState;
	BuitState* _sequenceSelectState;
	BuitState* _sequenceSettingsState;

public:
	BuitStateMachine(BuitDevicesManager& outDevices);  //, LiquidScreen& lScreen
	void singleClick();
	void doubleClick();
	void longClick();
	void rotaryTurned(ControlCommand command);
	void threeAxisChanged(ControlCommand command);
	void trellisPressed(ControlCommand command);
	void trellisReleased(ControlCommand command);
	void sequencerCallback(ControlCommand command);

	void setState(BuitState* state);

	BuitState* getInitState();

	BuitState* getTransportState();

	BuitState* getGlobalSettingsState();

	BuitState* getSceneEditState();

	BuitState* getSceneSettingsState();

	BuitState* getSequenceEditState();

	BuitState* getSequencePianoRollState();

	BuitState* getSequenceSelectState();

	BuitState* getSequenceSettingsState();
};
