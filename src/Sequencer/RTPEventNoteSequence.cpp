#include <RTPEventNoteSequence.h>
#include "RTPEventNotePlus.h"
#include "RTPTypeColors.h"
#include "ReMap.hpp"
#include <cstdint>
#include "Midi/MidiRouter.hpp"
#include "Midi/MidiMessage.hpp"

MidiRouter* RTPEventNoteSequence::_router = nullptr;

void RTPEventNoteSequence::setRouter(MidiRouter* router) {
    _router = router;
}


RTPEventNoteSequence::RTPEventNoteSequence(uint8_t midiChannel, uint16_t NEvents, uint8_t type, uint8_t baseNote, NotesPlayer& notesPlayer, MusicManager& musicManager):
 _notesPlayer(notesPlayer), _musicManager(musicManager){
  RTPParameter parameterType = RTPParameter(0, N_TYPES-1, type, "Type");
  RTPParameter parameterMidiChannel = RTPParameter(1, N_MIDI_CHANNELS, midiChannel, "Midi CH");
  RTPParameter parameterColor = RTPParameter(0, N_COLORS-1, 0, "Color");
  RTPParameter parameterLenght = RTPParameter(1, N_PAGES, 0, "Lenght");
  RTPParameter parameterInput = RTPParameter(0, 8, 0, "Input");
  RTPParameter parameterPort = RTPParameter(0, 8, 0, "Output");
  sequenceParameters.push_back(parameterType);
  sequenceParameters.push_back(parameterMidiChannel);
  sequenceParameters.push_back(parameterColor);
  sequenceParameters.push_back(parameterLenght);
  sequenceParameters.push_back(parameterInput);
  sequenceParameters.push_back(parameterPort);
  _baseNote = baseNote;
  _currentPosition = 0;
  _isRecording = false;
  _isEnabled = true;
  _selectedParameter = 0;
  _selectedPage = 0;
  _pages = ceil(NEvents / SEQ_BLOCK_SIZE);
  for(uint16_t i=0; i < NEvents; i++){
    RTPEventNotePlus eventNote = RTPEventNotePlus(midiChannel, false, _baseNote , 80); // true, 60, 80
    addEventNote(eventNote);
  }
}

void RTPEventNoteSequence::clearSequence(){
  EventNoteSequence.clear();
}

void RTPEventNoteSequence::addEventNote(RTPEventNotePlus eventNote){
  EventNoteSequence.push_back(eventNote);
}

void RTPEventNoteSequence::fordwardSequence(){
  _currentPosition++;
  if(_currentPosition >= getSequenceSize())
    _currentPosition = 0;
}

void RTPEventNoteSequence::backwardSequence(){
  _currentPosition--;
  if(_currentPosition < 0)
    _currentPosition = getSequenceSize();
}

void RTPEventNoteSequence::resetSequence(){
  _currentPosition = 0;
}

uint16_t RTPEventNoteSequence::getCurrentSequencePosition(){
  return _currentPosition;
}

bool RTPEventNoteSequence::isCurrentSequenceEnabled(){
  return _isEnabled;
}

bool RTPEventNoteSequence::isRecording(){
  return _isRecording;
}

void RTPEventNoteSequence::toggleRecording(){
  _isRecording = !_isRecording;
}

void RTPEventNoteSequence::enableSequence(bool state){
  _isEnabled = state;
}

void RTPEventNoteSequence::selectParameter(uint8_t parameterIndex){
  if(parameterIndex >= sequenceParameters.size() - 1)
    parameterIndex = sequenceParameters.size() - 1;
  _selectedParameter = parameterIndex;
  Serial.printf("Selected parameter: %d\n", _selectedParameter);
}

void RTPEventNoteSequence::increaseParameterValue(){
  sequenceParameters[_selectedParameter].incValue();
}

void RTPEventNoteSequence::decreaseParameterValue(){
  sequenceParameters[_selectedParameter].decValue();
}

void RTPEventNoteSequence::increasePage(){
  if(_selectedPage < _pages - 1)
    _selectedPage++;
}

void RTPEventNoteSequence::decreasePage(){
  if(_selectedPage > 0)
    _selectedPage--;
}

uint8_t RTPEventNoteSequence::getParameterValue(){
  return sequenceParameters[_selectedParameter].getValue();
}

String RTPEventNoteSequence::getParameterName(){
  return sequenceParameters[_selectedParameter].getName();
}

void RTPEventNoteSequence::setMidiChannel(uint8_t midiChannel){
  sequenceParameters[MIDI_CHANNEL].setValue(midiChannel);
}

uint8_t RTPEventNoteSequence::getMidiChannel(){
  return sequenceParameters[MIDI_CHANNEL].getValue();
}

uint8_t RTPEventNoteSequence::getMidiChannel() const {
  return sequenceParameters[MIDI_CHANNEL].getValue();
}

void RTPEventNoteSequence::setColor(uint32_t color){
  sequenceParameters[COLOR].setValue(color);
}
	
uint32_t RTPEventNoteSequence::getColor(){
  return sequenceParameters[COLOR].getValue();
}

void RTPEventNoteSequence::setType(uint8_t type){
  sequenceParameters[TYPE].setValue(type);
}
uint8_t RTPEventNoteSequence::getType(){
  return sequenceParameters[TYPE].getValue();
}

uint8_t RTPEventNoteSequence::getType() const {
  return sequenceParameters[TYPE].getValue();
}

uint8_t RTPEventNoteSequence::getPort(){
  return sequenceParameters[PORT].getValue();
}

uint8_t RTPEventNoteSequence::getPort() const {
  return sequenceParameters[PORT].getValue();
}

void RTPEventNoteSequence::setPort(uint8_t port){
  sequenceParameters[PORT].setValue(port);
}

MidiPort RTPEventNoteSequence::getPortAsMidiPort(){
  switch(sequenceParameters[PORT].getValue()){
    case 0: return MidiPort::NONE;        // Default: use routing table
    case 1: return MidiPort::USB_DEVICE;
    case 2: return MidiPort::USB_HOST;    // Broadcast to all USB Host devices
    case 3: return MidiPort::DIN;
    case 4: return MidiPort::USB_DEVICE | MidiPort::USB_HOST | MidiPort::DIN;  // ALL external
    case 5: return MidiPort::USB_HOST;    // USB Host device 1
    case 6: return MidiPort::USB_HOST;    // USB Host device 2
    case 7: return MidiPort::USB_HOST;    // USB Host device 3
    case 8: return MidiPort::USB_HOST;    // USB Host device 4
    default: return MidiPort::NONE;
  }
}

uint8_t RTPEventNoteSequence::getUsbHostDeviceIndex(){
  uint8_t port = sequenceParameters[PORT].getValue();
  if (port >= 5 && port <= 8) return port - 5;  // 0-3
  return 0xFF;  // broadcast
}

uint8_t RTPEventNoteSequence::getInput(){
  return sequenceParameters[INPUT_PORT].getValue();
}

uint8_t RTPEventNoteSequence::getInput() const {
  return sequenceParameters[INPUT_PORT].getValue();
}

void RTPEventNoteSequence::setInput(uint8_t input){
  sequenceParameters[INPUT_PORT].setValue(input);
}

bool RTPEventNoteSequence::acceptsInput(uint8_t srcPort, uint8_t srcDevice){
  uint8_t inp = sequenceParameters[INPUT_PORT].getValue();
  if (inp == 0) return true;  // Any — accept everything
  // Map input parameter to expected source port
  // 1=USB_DEVICE(0x01), 2=USB_HOST(0x02), 3=DIN(0x04), 4=ALL(accept any)
  // 5-8=USB Host device 1-4
  switch (inp) {
    case 1: return srcPort == static_cast<uint8_t>(MidiPort::USB_DEVICE);
    case 2: return srcPort == static_cast<uint8_t>(MidiPort::USB_HOST);
    case 3: return srcPort == static_cast<uint8_t>(MidiPort::DIN);
    case 4: return true;  // ALL — accept any source
    case 5: case 6: case 7: case 8:
      return srcPort == static_cast<uint8_t>(MidiPort::USB_HOST)
             && srcDevice == (inp - 5);
    default: return true;
  }
}

size_t RTPEventNoteSequence::getSequenceSize(){
  return EventNoteSequence.size();
}

void RTPEventNoteSequence::editNoteInSequence(size_t position, bool eventState){
  position = position + pageOffset();
  if(position < EventNoteSequence.size()){
    pointIterator(position);
    it->setEventState(eventState);
  }
}

bool RTPEventNoteSequence::getNoteStateInSequence(size_t position){
  position = position + pageOffset();
  if(position < EventNoteSequence.size()){
    pointIterator(position);
    return it->eventState();
  } 
  return false;
}

uint8_t RTPEventNoteSequence::getNoteVelocityInSequence(size_t position){
  position = position + pageOffset();
  if(position < EventNoteSequence.size()){
    pointIterator(position);
    return it->getEventVelocity();
  } 
  return 0;
}

void RTPEventNoteSequence::editNoteInSequence(size_t position, uint8_t note, uint8_t velocity){
  position = position + pageOffset();
  if(position < EventNoteSequence.size()){
    pointIterator(position);
    it->setEventNote(note);
    it->setEventVelocity(velocity);
  }
}

void RTPEventNoteSequence::editNoteInSequence(size_t position, uint8_t note, uint8_t velocity, uint8_t length, bool literalPitch){
  position = position + pageOffset();
  if(position < EventNoteSequence.size()){
    pointIterator(position);
    it->setEventNote(note);
    it->setEventVelocity(velocity);
    it->setLength(length);
    it->setLiteralPitch(literalPitch);
  }
}

void RTPEventNoteSequence::resizeSequence(size_t newSize){
  if(newSize > EventNoteSequence.size()){
    for(size_t i=EventNoteSequence.size(); i < newSize; i++){
      RTPEventNotePlus eventNote = RTPEventNotePlus(sequenceParameters[MIDI_CHANNEL].getValue(),false, 60, 80);
      EventNoteSequence.push_back(eventNote);
    }
  }
  else if(newSize < EventNoteSequence.size()){
    while(EventNoteSequence.size() > newSize)
      EventNoteSequence.pop_back();
  }
}

uint16_t RTPEventNoteSequence::pageOffset(){
  return _selectedPage * SEQ_BLOCK_SIZE;
}

uint8_t RTPEventNoteSequence::page(){
  return _selectedPage;
}

list<RTPEventNotePlus> RTPEventNoteSequence::getEventNoteSequence(){
  return EventNoteSequence;
}

const list<RTPEventNotePlus>& RTPEventNoteSequence::getEventNoteSequence() const {
  return EventNoteSequence;
}

String RTPEventNoteSequence::dumpSequenceToJson(){
  String noteSeqString;
  StaticJsonDocument<1536> doc;
  doc["type"] = getType();
  doc["ch"] = getMidiChannel();
  JsonArray seq = doc.createNestedArray("seq");
  for (RTPEventNotePlus eventNote : EventNoteSequence){
    JsonObject note = seq.createNestedObject();
    note["read"] = eventNote.getEventRead();
    note["vel"] = eventNote.eventState() ? eventNote.getEventVelocity() : 0;
    note["len"] = eventNote.getLength();
  }
  serializeJsonPretty(doc, noteSeqString);
  Serial.printf("%s\n", noteSeqString.c_str());
  return noteSeqString;
}

void RTPEventNoteSequence::pointIterator(uint16_t position){
  it = EventNoteSequence.begin();
  advance(it, position);
}

void RTPEventNoteSequence::routeLiveNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) {
    if (_router) {
        MidiMessage msg { MidiMessage::NoteOn, channel, note, velocity, MidiPort::INTERNAL };
        msg.destOverride = getPortAsMidiPort();
        msg.usbHostIndex = getUsbHostDeviceIndex();
        _router->route(msg);
    } else if (_midiOutput) {
        _midiOutput->sendNoteOn(note, velocity, channel);
    }
}

void RTPEventNoteSequence::routeLiveNoteOff(uint8_t note, uint8_t channel) {
    if (_router) {
        MidiMessage msg { MidiMessage::NoteOff, channel, note, 0, MidiPort::INTERNAL };
        msg.destOverride = getPortAsMidiPort();
        msg.usbHostIndex = getUsbHostDeviceIndex();
        _router->route(msg);
    } else if (_midiOutput) {
        _midiOutput->sendNoteOff(note, 0, channel);
    }
}

void RTPEventNoteSequence::routeLiveCC(uint8_t controller, uint8_t value, uint8_t channel) {
    if (_router) {
        MidiMessage msg { MidiMessage::ControlChange, channel, controller, value, MidiPort::INTERNAL };
        msg.destOverride = getPortAsMidiPort();
        msg.usbHostIndex = getUsbHostDeviceIndex();
        _router->route(msg);
    } else if (_midiOutput) {
        _midiOutput->sendControlChange(controller, value, channel);
    }
}