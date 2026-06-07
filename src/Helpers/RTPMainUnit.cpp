#include "RTPMainUnit.hpp"
#include "ControlCommand.h"

RTPMainUnit::RTPMainUnit(){
}

void RTPMainUnit::begin(){  
  Serial.begin(9600);
  Wire.begin();
  Wire1.begin();
  vlSensor.initSetup();
  vlSensor.startContinuous();
  rtpTrellis.begin(this);
  SequencerManager.begin(this);
  devicesManager.initSetup();
  devicesManager.setClockGenerator(clockGenerator);  // Link clock generator to devices manager
}

void RTPMainUnit::update(){
  rtpRotary.callbackFromRotary(this);
  rtpRotary.callbackFromClicks(this);
  rtpTrellis.read();
}

void RTPMainUnit::updatePeriodically(){
  vlSensor.callbackThreeAxisChanged(this);
}

void RTPMainUnit::actOnControlsCallback(ControlCommand callbackCommand){
  //Serial.printf("DEVICE: %d  TYPE: %d  VALUE: %d\n", callbackCommand.controlType, callbackCommand.commandType, callbackCommand.value);
  stateMachineManager.handleActions(callbackCommand);
}

void RTPMainUnit::actOnSequencerCallback(ControlCommand callbackCommand){
  //Serial.printf("Seq Callback  TYPE: %d  VALUE: %d\n", callbackCommand.commandType, callbackCommand.value);
  devicesManager.recorderAdvanceTick();
  stateMachineManager.handleActions(callbackCommand);
}

void RTPMainUnit::linkToSequencerManager(uint8_t realtimebyte){
  // Only process external MIDI when in External mode
  if (clockGenerator.getMode() == rtp::SyncMode::External) {
    SequencerManager.handleRealTimeSystem(realtimebyte);
  }
}

void RTPMainUnit::updateClockGenerator(){
  // Only process internal clock when in Internal mode
  if (clockGenerator.getMode() != rtp::SyncMode::Internal) {
    return;
  }
  
  // Update clock generator - returns true when clock pulse fires
  if (clockGenerator.update()) {
    // Send CLOCK (0xF8) to sequencer manager
    SequencerManager.handleRealTimeSystem(0xF8);  // CLOCK
    
    // Optionally send MIDI clock to external gear
    if (clockGenerator.isSendingMidiRealtime()) {
      usbMIDI.sendRealTime(0xF8);  // USB MIDI clock
      Serial1.write(0xF8);  // Hardware MIDI clock
    }
    
    clockGenerator.clearPendingClock();
  }
  
  // Check for transport events
  if (clockGenerator.shouldSendStart()) {
    SequencerManager.handleRealTimeSystem(0xFA);  // START
    if (clockGenerator.isSendingMidiRealtime()) {
      usbMIDI.sendRealTime(0xFA);  // USB MIDI start
      Serial1.write(0xFA);  // Hardware MIDI start
    }
    clockGenerator.clearPendingStart();
    
    // Send immediate CLOCK to trigger first step without delay
    // This eliminates the perceptual lag between pressing play and hearing sound
    SequencerManager.handleRealTimeSystem(0xF8);  // Immediate CLOCK
    if (clockGenerator.isSendingMidiRealtime()) {
      usbMIDI.sendRealTime(0xF8);  // USB MIDI clock
      Serial1.write(0xF8);  // Hardware MIDI clock
    }
  }
  
  if (clockGenerator.shouldSendStop()) {
    SequencerManager.handleRealTimeSystem(0xFC);  // STOP
    if (clockGenerator.isSendingMidiRealtime()) {
      usbMIDI.sendRealTime(0xFC);  // USB MIDI stop
      Serial1.write(0xFC);  // Hardware MIDI stop
    }
    clockGenerator.clearPendingStop();
  }
}

void RTPMainUnit::routeControlChange(uint8_t channel, uint8_t control, uint8_t value) {
  musicManager.setCurrentHarmony(channel, control, value);
  ControlCommand command = ControlCommand{MIDI_CC, control, value};
  stateMachineManager.handleActions(command);
}

void RTPMainUnit::routeNoteOnOff(uint8_t channel, uint8_t note, uint8_t velocity){
  // Create a control command with different control types for note-on and note-off
  // For note-on: controlType = MIDI_NOTE (7)
  // For note-off: controlType = MIDI_NOTE + 100 (107)
  int controlType = (velocity > 0) ? MIDI_NOTE : MIDI_NOTE + 100;
  
  ControlCommand command = ControlCommand{controlType, note, velocity};
  stateMachineManager.handleActions(command, channel);
}