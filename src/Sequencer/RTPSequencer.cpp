#include "RTPSequencer.h"
//#include "RTPMainUnit.hpp"

//RTPMainUnit* RTPSequencer::mainUnit;

RTPSequencer::RTPSequencer(int NScenes, MusicManager& musicManager) :_musicManager(musicManager) {
  Serial.println("CREATING RTPSequencer");
  _NScenes = NScenes;
  _selectedScene = 0;
  for(int i=0; i < _NScenes; i++){
    RTPScene scene = RTPScene("Scene", SCENE_BLOCK_SIZE, i, _notesPlayer, _musicManager);
    Sequencer.push_back(scene);
  }
}

/*void RTPSequencer::connectMainUnit(RTPMainUnit* mainUnit){
  _notesPlayer.connectMainUnit(mainUnit);
}*/

void RTPSequencer::playAndMoveSequencer(){
  for(size_t i=0; i<Sequencer.size(); i++){ 
    Sequencer[i].playScene();
    _notesPlayer.playNotes();
    Sequencer[i].fordwardScene();
    _notesPlayer.decreaseTimeToLive();
  }
}

void RTPSequencer::stopAndCleanSequencer(){
  for(size_t i=0; i<Sequencer.size(); i++)
    Sequencer[i].resetScene();
  _notesPlayer.killAllNotes();
}

void RTPSequencer::pauseSequencer(){
  _notesPlayer.killAllNotes();
}

int RTPSequencer::getSelectedSequencePosition(){
  return Sequencer[_selectedScene].getSelectedSequenceCurrentPosition();
}

int RTPSequencer::getSelectedSequencePageOffset(){
  return Sequencer[_selectedScene].getSelectedSequencePageOffset();
}

int RTPSequencer::getSelectedSequencePage(){
  return Sequencer[_selectedScene].getSelectedSequencePage();
}

void RTPSequencer::selectScene(int scene){
  _selectedScene = scene;
}

void RTPSequencer::increaseSelectedScene(){
  _selectedScene++;
  if(_selectedScene >= _NScenes - 1)
    _selectedScene = _NScenes - 1;
}

void RTPSequencer::decreaseSelectedScene(){
  _selectedScene--;
  if(_selectedScene <= 0)
    _selectedScene = 0;
}

int RTPSequencer::getSelectScene(){
  return _selectedScene;
}

void RTPSequencer::addScene(RTPScene scene){
  Sequencer.push_back(scene);
}

void RTPSequencer::removeScene(int scene){
  //Sequencer.erase(Sequencer.begin() + scene);
}

void RTPSequencer::toggleNoteInSceneInSelectedSequence(int position){
  Sequencer[_selectedScene].toggleNoteInSequence(position);
}

void RTPSequencer::toggleSequence(int sequenceIndex){
  Sequencer[_selectedScene].toggleSequence(sequenceIndex);
}

RTPSequencesState RTPSequencer::getSequencesState(){
  RTPSequencesState sequencesState = Sequencer[_selectedScene].getSequencesState();
  return sequencesState;
}

void RTPSequencer::selectSequence(int sequenceIndex){
  Sequencer[_selectedScene].setSelectedSequence(sequenceIndex);
}

int RTPSequencer::getSelectedSequence(){
  return Sequencer[_selectedScene].getSelectedSequence();
}

void RTPSequencer::selectParameterInSequece(int parameterIndex){
  Sequencer[_selectedScene].selectParameterInSequece(parameterIndex);
}

int RTPSequencer::getSelectedParameterInSequeceValue(){
  return Sequencer[_selectedScene].getSelectedParameterInSequeceValue();
}

String RTPSequencer::getSelectedParameterInSequeceName(){
  return Sequencer[_selectedScene].getSelectedParameterInSequeceName();
}

void RTPSequencer::incselectParameterInSequece(){
  Sequencer[_selectedScene].incselectParameterInSequece();
}

void RTPSequencer::decselectParameterInSequece(){
  Sequencer[_selectedScene].decselectParameterInSequece();
}

SequenceSettings RTPSequencer::getSelectedSequenceSettings(){
  SequenceSettings sequenceSettings = Sequencer[_selectedScene].getSelectedSequenceSettings();
  return sequenceSettings; 
}

RTPSequenceNoteStates RTPSequencer::getSelectedSequenceNoteStates(){
  RTPSequenceNoteStates sequenceNoteStates = Sequencer[_selectedScene].getSequenceNoteStates();
  return sequenceNoteStates;
}

void RTPSequencer::nudgePageInSelectedSequence(ControlCommand command){
  switch(command.commandType){
    case ROTATING_LEFT:
      Sequencer[_selectedScene].decselectPageInSequence();
      return;
    case ROTATING_RIGHT:
      Sequencer[_selectedScene].incselectPageInSequence();
      return;
  }
}

void RTPSequencer::editNoteInCurrentPosition(ControlCommand command){
  Sequencer[_selectedScene].editNoteInCurrentPosition(command);
}

int RTPSequencer::getSelectedSequenceColor(){
  return Sequencer[_selectedScene].getSequenceColor();
}

void RTPSequencer::dumpSequencesToJson(){
  for(size_t i=0; i<Sequencer.size(); i++)
    Sequencer[i].dumpSequencesToJson();
}