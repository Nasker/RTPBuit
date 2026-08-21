#include "Arduino.h"
#include "RTPMainUnit.hpp"
#include "RTPPeriodicBang.h"
#include "USBHost_t36.h"
#include "Midi/MidiMessage.hpp"
#include "Midi/MidiParser.hpp"

#define UPDATE_PERIOD 10

RTPMainUnit mUnit;
RTPPeriodicBang periodicUpdate(UPDATE_PERIOD);
MidiParser dinParser(MidiPort::DIN);
USBHost myusb;
MIDIDevice midi1(myusb);
MIDIDevice midi2(myusb);
MIDIDevice midi3(myusb);
MIDIDevice midi4(myusb);

void actOnPeriodicUpdate(String callbackString){
  mUnit.updatePeriodically();
}

// --- USB Device MIDI callbacks (source = USB_DEVICE) ---

void usbDeviceRealTime(uint8_t realtimebyte){
  MidiMessage msg { MidiMessage::RealTime, 0, realtimebyte, 0, MidiPort::USB_DEVICE };
  mUnit.getMidiRouter().route(msg);
}

void usbDeviceControlChange(uint8_t channel, uint8_t control, uint8_t value){
  MidiMessage msg { MidiMessage::ControlChange, channel, control, value, MidiPort::USB_DEVICE };
  mUnit.getMidiRouter().route(msg);
}

void usbDeviceNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
  MidiMessage msg { MidiMessage::NoteOn, channel, note, velocity, MidiPort::USB_DEVICE };
  mUnit.getMidiRouter().route(msg);
}

void usbDeviceNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) {
  MidiMessage msg { MidiMessage::NoteOff, channel, note, velocity, MidiPort::USB_DEVICE };
  mUnit.getMidiRouter().route(msg);
}

// --- USB Host MIDI callbacks (source = USB_HOST) ---

void usbHostControlChange(uint8_t channel, uint8_t control, uint8_t value){
  MidiMessage msg { MidiMessage::ControlChange, channel, control, value, MidiPort::USB_HOST };
  mUnit.getMidiRouter().route(msg);
}

void usbHostNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
  MidiMessage msg { MidiMessage::NoteOn, channel, note, velocity, MidiPort::USB_HOST };
  mUnit.getMidiRouter().route(msg);
}

void usbHostNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) {
  MidiMessage msg { MidiMessage::NoteOff, channel, note, velocity, MidiPort::USB_HOST };
  mUnit.getMidiRouter().route(msg);
}

void usbHostRealTime(uint8_t realtimebyte) {
  MidiMessage msg { MidiMessage::RealTime, 0, realtimebyte, 0, MidiPort::USB_HOST };
  mUnit.getMidiRouter().route(msg);
}

void setup() {
  mUnit.begin();
  myusb.begin();
  
  // Wire all 4 USB Host MIDI devices into output and manager
  MIDIDevice* midiDevices[] = { &midi1, &midi2, &midi3, &midi4 };
  mUnit.getUsbHostManager().begin(myusb);
  for (uint8_t i = 0; i < 4; i++) {
    mUnit.setUsbHostDevice(midiDevices[i], i);
    mUnit.getUsbHostManager().addDevice(midiDevices[i], i);
    midiDevices[i]->setHandleControlChange(usbHostControlChange);
    midiDevices[i]->setHandleNoteOn(usbHostNoteOn);
    midiDevices[i]->setHandleNoteOff(usbHostNoteOff);
    midiDevices[i]->setHandleRealTimeSystem(usbHostRealTime);
  }
  
  // USB MIDI handlers (tagged as USB_DEVICE source)
  usbMIDI.setHandleRealTimeSystem(usbDeviceRealTime);
  usbMIDI.setHandleControlChange(usbDeviceControlChange);
  usbMIDI.setHandleNoteOn(usbDeviceNoteOn);
  usbMIDI.setHandleNoteOff(usbDeviceNoteOff);
  
  // Hardware Serial MIDI on Serial1 (5-pin DIN) - pins 0=RX, 1=TX
  Serial1.begin(31250);  // Standard MIDI baud rate
}

void loop() {
  usbMIDI.read();
  mUnit.update();
  myusb.Task();
  midi1.read();
  midi2.read();
  midi3.read();
  midi4.read();
  mUnit.getUsbHostManager().update();
  
  // Parse hardware serial MIDI (5-pin DIN on Serial1)
  while (Serial1.available()) {
    uint8_t inByte = Serial1.read();
    if (dinParser.parse(inByte)) {
      mUnit.getMidiRouter().route(dinParser.getMessage());
    }
  }
  
  periodicUpdate.callbackPeriodBang(actOnPeriodicUpdate);
}