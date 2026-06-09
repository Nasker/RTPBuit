#include "Arduino.h"

#include "SceneSettingsState.h"
#include "PatternBankState.h"

SceneSettingsState::SceneSettingsState (BuitStateMachine& buitMachine, BuitDevicesManager& devices) : BuitState(devices), _buitMachine(buitMachine) {
  Serial.println("SceneSettingsState");
}

void SceneSettingsState::singleClick() {
  _focusedPad = -1;
  _devices.presentSceneSettings();
}

void SceneSettingsState::doubleClick() {
  _focusedPad = -1;
  _buitMachine.setState(_buitMachine.getSceneEditState());
  _devices.presentScene();
}

void SceneSettingsState::tripleClick() {
  PatternBankState* bankState = static_cast<PatternBankState*>(_buitMachine.getPatternBankLoadState());
  bankState->enter();
  _buitMachine.setState(bankState);
}

void SceneSettingsState::longClick() {
  PatternBankState* bankState = static_cast<PatternBankState*>(_buitMachine.getPatternBankSaveState());
  bankState->enter();
  _buitMachine.setState(bankState);
}

void SceneSettingsState::rotaryTurned(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void SceneSettingsState::threeAxisChanged(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

static const char* padLabel(int8_t pad) {
  switch (pad) {
    case 0: return "Load pattern";
    case 1: return "Save pattern";
    case 2: return "Add scene";
    case 3: return "Remove scene";
    case 4: return "Toggle all seq";
    default: return "";
  }
}

static const char* padHint(int8_t pad) {
  switch (pad) {
    case 0: return "Opens load bank";
    case 1: return "Opens save bank";
    case 2: return "Blank MONO scene";
    case 3: return "Del current scene";
    case 4: return "Mute/unmute all";
    default: return "";
  }
}

void SceneSettingsState::trellisPressed(ControlCommand command) {
  int8_t pad = (int8_t)command.value;
  if (pad < 0 || pad > 4) return;

  if (_focusedPad != pad) {
    // First press: focus this pad — highlight it and describe on OLED
    _focusedPad = pad;
    _devices.presentSceneSettings(_focusedPad);
    _devices.printToScreen(padLabel(pad), padHint(pad), "Press again to run");
    return;
  }

  // Second press on same pad: execute
  _focusedPad = -1;
  switch (pad) {
    case 0: {
      PatternBankState* bankState = static_cast<PatternBankState*>(_buitMachine.getPatternBankLoadState());
      bankState->enter();
      _buitMachine.setState(bankState);
      break;
    }
    case 1: {
      PatternBankState* bankState = static_cast<PatternBankState*>(_buitMachine.getPatternBankSaveState());
      bankState->enter();
      _buitMachine.setState(bankState);
      break;
    }
    case 2:
      _devices.sceneAdd();
      _devices.presentSceneSettings();
      break;
    case 3:
      _devices.sceneRemove();
      _devices.presentSceneSettings();
      break;
    case 4:
      _devices.sceneToggleAll();
      _devices.presentSceneSettings();
      break;
    default:
      break;
  }
}

void SceneSettingsState::trellisReleased(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void SceneSettingsState::sequencerCallback(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void SceneSettingsState::midiNote(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void SceneSettingsState::midiNoteOff(ControlCommand command) {
  // Simple implementation - just ignore note-off events
  // No need to do anything here
}

void SceneSettingsState::midiCC(ControlCommand command) {
  //Serial.println("Does nothing here!");
}