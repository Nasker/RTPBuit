#pragma once

#include "BuitState.h"
#include "BuitDevicesManager.hpp"
#include <memory>

class BuitStateMachine{
	BuitState* _state = nullptr;  // Non-owning observer pointer to active state
	std::unique_ptr<BuitState> _initState;
	std::unique_ptr<BuitState> _transportState;
	std::unique_ptr<BuitState> _globalSettingsState;
	std::unique_ptr<BuitState> _sceneEditState;
	std::unique_ptr<BuitState> _sceneSettingsState;
	std::unique_ptr<BuitState> _sequenceEditState;
	std::unique_ptr<BuitState> _sequencePianoRollState;
	std::unique_ptr<BuitState> _sequenceSelectState;
	std::unique_ptr<BuitState> _sequenceSettingsState;
	std::unique_ptr<BuitState> _patternBankLoadState;
	std::unique_ptr<BuitState> _patternBankSaveState;

public:
	BuitStateMachine(BuitDevicesManager& outDevices);
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
	BuitState* getPatternBankLoadState();
	BuitState* getPatternBankSaveState();
};
