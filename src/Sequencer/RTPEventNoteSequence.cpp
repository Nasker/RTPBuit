#include <RTPEventNoteSequence.h>
#include "RTPEventNotePlus.h"
#include "RTPTypeColors.h"
#include "ReMap.hpp"


RTPEventNoteSequence::RTPEventNoteSequence(int midiChannel, int NEvents, int type, int baseNote, NotesPlayer& notesPlayer, MusicManager& musicManager):
 _notesPlayer(notesPlayer), _musicManager(musicManager){
  RTPParameter parameterType = RTPParameter(0, N_TYPES-1, type, "Type");
  RTPParameter parameterMidiChannel = RTPParameter(1, N_MIDI_CHANNELS, midiChannel, "Midi CH");
  RTPParameter parameterColor = RTPParameter(0, N_COLORS-1, 0, "Color");
  RTPParameter parameterLenght = RTPParameter(1, N_PAGES, 0, "Lenght");
  sequenceParameters.push_back(parameterType);
  sequenceParameters.push_back(parameterMidiChannel);
  sequenceParameters.push_back(parameterColor);
  sequenceParameters.push_back(parameterLenght);
  _baseNote = baseNote;
  _currentPosition = 0;
  _isRecording = false;
  _isEnabled = true;
  _selectedParameter = 0;
  _selectedPage = 0;
  _pages = ceil(NEvents / SEQ_BLOCK_SIZE);
  for(int i=0; i < NEvents; i++){
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

int RTPEventNoteSequence::getCurrentSequencePosition(){
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

void RTPEventNoteSequence::selectParameter(int parameterIndex){
  if(parameterIndex >= 3)
    parameterIndex = 3;
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
  _selectedPage++;
  if(_selectedPage >= _pages)
    _selectedPage = _pages - 1;
}

void RTPEventNoteSequence::decreasePage(){
  _selectedPage--;
  if(_selectedPage < 0)
    _selectedPage = 0;
}

int RTPEventNoteSequence::getParameterValue(){
  return sequenceParameters[_selectedParameter].getValue();
}

String RTPEventNoteSequence::getParameterName(){
  return sequenceParameters[_selectedParameter].getName();
}

void RTPEventNoteSequence::setMidiChannel(int midiChannel){
  sequenceParameters[MIDI_CHANNEL].setValue(midiChannel);
}

int RTPEventNoteSequence::getMidiChannel(){
  return sequenceParameters[MIDI_CHANNEL].getValue();
}

int RTPEventNoteSequence::getMidiChannel() const {
  return sequenceParameters[MIDI_CHANNEL].getValue();
}

void RTPEventNoteSequence::setColor(int color){
  sequenceParameters[COLOR].setValue(color);
}
	
int RTPEventNoteSequence::getColor(){
  return sequenceParameters[COLOR].getValue();
}

void RTPEventNoteSequence::setType(int type){
  sequenceParameters[TYPE].setValue(type);
}
int RTPEventNoteSequence::getType(){
  return sequenceParameters[TYPE].getValue();
}

int RTPEventNoteSequence::getType() const {
  return sequenceParameters[TYPE].getValue();
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
  return NULL;
}

int RTPEventNoteSequence::getNoteVelocityInSequence(size_t position){
  position = position + pageOffset();
  if(position < EventNoteSequence.size()){
    pointIterator(position);
    return it->getEventVelocity();
  } 
  return NULL;
}

void RTPEventNoteSequence::editNoteInSequence(size_t position, int note, int velocity){
  position = position + pageOffset();
  if(position < EventNoteSequence.size()){
    pointIterator(position);
    it->setEventNote(note);
    it->setEventVelocity(velocity);
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

int RTPEventNoteSequence::pageOffset(){
  return _selectedPage * SEQ_BLOCK_SIZE;
}

int RTPEventNoteSequence::page(){
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

void RTPEventNoteSequence::pointIterator(int position){
  it = EventNoteSequence.begin();
  advance(it, position);
}