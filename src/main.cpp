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
  usbMIDI.setHandleRealTimeSystem(linkToSequencerManager);
  usbMIDI.setHandleControlChange(routeControlChange);
  usbMIDI.setHandleNoteOn(routeNoteOn);
  usbMIDI.setHandleNoteOff(routeNoteOff);
  midi1.setHandleControlChange(routeControlChange);
  midi1.setHandleNoteOn(routeNoteOn);
  midi1.setHandleNoteOff(routeNoteOff);
}

void loop() {
  usbMIDI.read();
  mUnit.update();
  myusb.Task();
	midi1.read();
  periodicUpdate.callbackPeriodBang(actOnPeriodicUpdate);
}