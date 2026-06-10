#include "Arduino.h"
#include "TransportState.h"
#include "constants.h"

TransportState::TransportState (BuitStateMachine& buitMachine, BuitDevicesManager& devices) : BuitState(devices), _buitMachine(buitMachine) {
  Serial.println("TransportState");
  _buitMachine = buitMachine;
}

void TransportState::onEnter() {
  _devices.presentTransport();
}

void TransportState::singleClick() {
  // Single click: could be used for quick action on last selected parameter
  // For now, refresh the transport display
  _devices.presentTransport();
}

void TransportState::doubleClick() {
  _buitMachine.setState(_buitMachine.getSceneEditState());
}

void TransportState::tripleClick() {
  //Serial.println("Does nothing here!");
}

void TransportState::longClick() {
  // Long press: could reset the last adjusted parameter to default
  // For Tap tempo (BPM): reset to 120 BPM
  if (_lastPressedButton == 3) {
    _devices.transportSetBPM(120.0f);
    _devices.presentTransport();
  }
  // For Rec (quantization): reset to 50%
  else if (_lastPressedButton == 2) {
    _devices.setQuantizeStrength(50);
    _devices.presentTransport();
  }
  else {
    _buitMachine.setState(_buitMachine.getGlobalSettingsState());
  }
}

void TransportState::rotaryTurned(ControlCommand command) {
  // Rotary adjusts values based on last pressed button
  // command.commandType contains ROTATING_LEFT or ROTATING_RIGHT
  switch (_lastPressedButton) {
    case 0:  // Play button - adjust swing/groove (0-100%)
      if (command.commandType == ROTATING_RIGHT) {
        _devices.incrementSwing(5);
      } else if (command.commandType == ROTATING_LEFT) {
        _devices.incrementSwing(-5);
      }
      _devices.presentTransport();
      break;
      
    case 2:  // Rec button - adjust quantization strength (0-100%)
      if (command.commandType == ROTATING_RIGHT) {
        _devices.incrementQuantizeStrength(5);
      } else if (command.commandType == ROTATING_LEFT) {
        _devices.incrementQuantizeStrength(-5);
      }
      _devices.presentTransport();
      break;
      
    case 3:  // Tap button - adjust BPM (most useful!)
      if (command.commandType == ROTATING_RIGHT) {
        _devices.transportIncrementBPM(1.0f);
      } else if (command.commandType == ROTATING_LEFT) {
        _devices.transportIncrementBPM(-1.0f);
      }
      _devices.presentTransport();
      break;
      
    case 4:  // Mode button - adjust master volume/transpose
      if (command.commandType == ROTATING_RIGHT) {
        _devices.incrementMasterVolume(5);
      } else if (command.commandType == ROTATING_LEFT) {
        _devices.incrementMasterVolume(-5);
      }
      _devices.presentTransport();
      break;
      
    default:
      // No button pressed yet, or Play/Stop - do nothing
      break;
  }
}

void TransportState::threeAxisChanged(ControlCommand command) {
  // CC Matrix functionality moved to new state (future: CCMatrixState)
  // _devices.sendBuitCC(command);
}

void TransportState::trellisPressed(ControlCommand command) {
  // Transport control button layout:
  // 0: Play, 1: Stop, 2: Rec, 3: Tap, 4: Mode
  
  uint8_t buttonId = command.value;  // Button index from trellis
  _lastPressedButton = buttonId;  // Remember for rotary control
  
  switch (buttonId) {
    case 0:  // Play
      if (_devices.isInternalClock()) {
        _devices.transportPlay();
      }
      _devices.presentTransport();
      break;
      
    case 1:  // Stop
      if (_devices.isInternalClock()) {
        _devices.transportStop();
      }
      _devices.presentTransport();
      break;
      
    case 2:  // Record (toggle recording on selected sequence)
      _devices.toggleSelectedSequenceRecording();
      _devices.presentTransport();
      break;
      
    case 3:  // Tap Tempo (only works in internal mode)
      if (_devices.isInternalClock()) {
        _devices.transportTapTempo();
      }
      _devices.presentTransport();
      break;
      
    case 4:  // Toggle Sync Mode (INT/EXT)
      _devices.transportToggleMode();
      _devices.presentTransport();
      break;
      
    default:
      // Other buttons not used in transport mode
      break;
  }
}

void TransportState::trellisReleased(ControlCommand command) {
  // Could use release for momentary actions
  // For now, just keep _lastPressedButton until another is pressed
}

void TransportState::sequencerCallback(ControlCommand command) {
  // Handle transport state changes to update display
  // Refresh transport display when transport events occur
  if (command.commandType == TRANSPORT_START || 
      command.commandType == TRANSPORT_STOP ||
      command.commandType == GRID_TICK) {
    // Only refresh periodically on GRID_TICK to avoid flicker
    if (command.commandType != GRID_TICK || command.value % 6 == 0) {
      _devices.presentTransport();
    }
  }
}

void TransportState::midiNote(ControlCommand command) {
  //Serial.println("Does nothing here!");
}

void TransportState::midiNoteOff(ControlCommand command) {
  // Simple implementation - just ignore note-off events
  // No need to do anything here
}

void TransportState::midiCC(ControlCommand command) {
  //Serial.println("Does nothing here!");
}