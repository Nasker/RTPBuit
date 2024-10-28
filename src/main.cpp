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

void linkToSequencerManager(byte realtimebyte){
  mUnit.linkToSequencerManager(realtimebyte);
}

void routeControlChange(byte channel, byte control, byte value){
  mUnit.routeControlChange(channel, control, value);
}

void routeNoteOn(byte channel, byte note, byte velocity) {
  mUnit.routeNoteOnOff(channel, note, velocity);
  usbMIDI.sendNoteOn(note, velocity, channel);
}

void routeNoteOff(byte channel, byte note, byte velocity) {
  mUnit.routeNoteOnOff(channel, note, velocity);
  usbMIDI.sendNoteOff(note, velocity, channel);
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