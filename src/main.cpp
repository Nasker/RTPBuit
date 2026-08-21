#include "Arduino.h"
#include "RTPMainUnit.hpp"
#include "RTPPeriodicBang.h"
#include "USBHost_t36.h"
#include "Midi/MidiMessage.hpp"

#define UPDATE_PERIOD 10

RTPMainUnit mUnit;
RTPPeriodicBang periodicUpdate(UPDATE_PERIOD);
USBHost myusb;
MIDIDevice midi1(myusb);

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
  
  // Wire USB Host device into MidiRouter output and manager
  mUnit.setUsbHostDevice(&midi1);
  mUnit.getUsbHostManager().begin(myusb, &midi1);
  
  // USB MIDI handlers (tagged as USB_DEVICE source)
  usbMIDI.setHandleRealTimeSystem(usbDeviceRealTime);
  usbMIDI.setHandleControlChange(usbDeviceControlChange);
  usbMIDI.setHandleNoteOn(usbDeviceNoteOn);
  usbMIDI.setHandleNoteOff(usbDeviceNoteOff);
  
  // USB Host MIDI handlers (tagged as USB_HOST source)
  midi1.setHandleControlChange(usbHostControlChange);
  midi1.setHandleNoteOn(usbHostNoteOn);
  midi1.setHandleNoteOff(usbHostNoteOff);
  midi1.setHandleRealTimeSystem(usbHostRealTime);
  
  // Hardware Serial MIDI on Serial1 (5-pin DIN) - pins 0=RX, 1=TX
  Serial1.begin(31250);  // Standard MIDI baud rate
}

void loop() {
  usbMIDI.read();
  mUnit.update();
  myusb.Task();
	midi1.read();
  mUnit.getUsbHostManager().update();
  
  // Read hardware serial MIDI (5-pin DIN on Serial1)
  while (Serial1.available()) {
    uint8_t inByte = Serial1.read();
    // Route real-time messages through the router (tagged as DIN source)
    if (inByte >= 0xF8) {
      MidiMessage msg { MidiMessage::RealTime, 0, inByte, 0, MidiPort::DIN };
      mUnit.getMidiRouter().route(msg);
    }
    // Note: Full MIDI parsing (Phase 6) will handle channel messages from DIN
  }
  
  periodicUpdate.callbackPeriodBang(actOnPeriodicUpdate);
}