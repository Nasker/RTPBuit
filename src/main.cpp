#include "Arduino.h"
#include "RTPMainUnit.hpp"
#include "RTPPeriodicBang.h"
#include "USBHost_t36.h"

#define UPDATE_PERIOD 10

RTPMainUnit mUnit;
RTPPeriodicBang periodicUpdate(UPDATE_PERIOD);
USBHost myusb;
MIDIDevice midi1(myusb);

void actOnPeriodicUpdate(String callbackString){
  mUnit.updatePeriodically();
}

void linkToSequencerManager(uint8_t realtimebyte){
  mUnit.linkToSequencerManager(realtimebyte);
}

void routeControlChange(uint8_t channel, uint8_t control, uint8_t value){
  mUnit.routeControlChange(channel, control, value);
}

void routeNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
  mUnit.routeNoteOnOff(channel, note, velocity);
  //usbMIDI.sendNoteOn(note, velocity, channel);
}

void routeNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) {
  mUnit.routeNoteOnOff(channel, note, 0);
  //usbMIDI.sendNoteOff(note, velocity, channel);
}

void setup() {
  mUnit.begin();
  myusb.begin();
  
  // USB MIDI handlers
  usbMIDI.setHandleRealTimeSystem(linkToSequencerManager);
  usbMIDI.setHandleControlChange(routeControlChange);
  usbMIDI.setHandleNoteOn(routeNoteOn);
  usbMIDI.setHandleNoteOff(routeNoteOff);
  
  // USB Host MIDI handlers (for USB MIDI devices connected to host port)
  midi1.setHandleControlChange(routeControlChange);
  midi1.setHandleNoteOn(routeNoteOn);
  midi1.setHandleNoteOff(routeNoteOff);
  
  // Hardware Serial MIDI on Serial1 (5-pin DIN) - pins 0=RX, 1=TX
  Serial1.begin(31250);  // Standard MIDI baud rate
}

void loop() {
  usbMIDI.read();
  mUnit.update();
  mUnit.updateClockGenerator();  // Process internal clock when in internal mode
  myusb.Task();
	midi1.read();
  
  // Read hardware serial MIDI (5-pin DIN on Serial1)
  while (Serial1.available()) {
    uint8_t byte = Serial1.read();
    // Route real-time messages to sequencer
    if (byte >= 0xF8) {
      linkToSequencerManager(byte);
    }
    // Note: Full MIDI parsing would need a state machine for channel messages
  }
  
  periodicUpdate.callbackPeriodBang(actOnPeriodicUpdate);
}