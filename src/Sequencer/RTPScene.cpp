#include <RTPScene.h>
#include "RTPSDManager.hpp"

int types[N_SCENES][SCENE_BLOCK_SIZE] = {
  {DRUM,DRUM,DRUM,DRUM,DRUM,DRUM,DRUM,DRUM,DRUM,DRUM,DRUM,DRUM,DRUM,DRUM,DRUM,DRUM},
  {BASS_SYNTH,BASS_SYNTH,BASS_SYNTH,BASS_SYNTH,MONO_SYNTH,MONO_SYNTH,MONO_SYNTH,MONO_SYNTH,MONO_SYNTH,MONO_SYNTH,POLY_SYNTH,POLY_SYNTH,POLY_SYNTH,POLY_SYNTH,POLY_SYNTH,POLY_SYNTH}
};

int midiChannels[N_SCENES][SCENE_BLOCK_SIZE] = {
  {10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10},
  {1,1,2,3,4,5,6,7,8,9,11,12,13,14,15,16}
};

RTPScene::RTPScene(String name, int NSequences, int scene, NotesPlayer& notesPlayer, MusicManager& musicManager) : _notesPlayer(notesPlayer), _musicManager(musicManager) {
  _name = name;
  _NSequences = NSequences;
  _selectedSequence = 0;
  for(int i=0; i < _NSequences; i++){
    int baseNote = types[scene][i] == DRUM ? 36 + i : 60;
    RTPEventNoteSequence sequence = RTPEventNoteSequence(midiChannels[scene][i], SEQ_BLOCK_SIZE * N_PAGES, types[scene][i], baseNote, _notesPlayer, _musicManager);
    SequencerScene.push_back(sequence);
  }
}

void RTPScene::playScene(){
  for(size_t i=0; i<SequencerScene.size(); i++)
    SequencerScene[i].playCurrentEventNote();
}

void RTPScene::fordwardScene(){
  for(size_t i=0; i<SequencerScene.size(); i++)
    SequencerScene[i].fordwardSequence();
}

void RTPScene::backwardScene(){
  for(size_t i=0; i<SequencerScene.size(); i++)
    SequencerScene[i].backwardSequence();
}

void RTPScene::resetScene(){
  for(size_t i=0; i<SequencerScene.size(); i++)
    SequencerScene[i].resetSequence();
}

void RTPScene::setSelectedSequence(int selectedSequence){
  _selectedSequence = selectedSequence;
}

int RTPScene::getSelectedSequence(){
  return _selectedSequence;
}

int RTPScene::getSelectedSequencePageOffset(){
  return SequencerScene[_selectedSequence].pageOffset();
}

int RTPScene::getSize(){
  return SequencerScene.size();
}

RTPSequenceNoteStates RTPScene::getSelectedSequenceNoteStates(){
  for(size_t i=0; i<SEQ_BLOCK_SIZE; i++){
    if(i < SequencerScene[_selectedSequence].getSequenceSize())
      _seqStates.val[i] = SequencerScene[_selectedSequence].getNoteStateInSequence(i);
    else
      _seqStates.val[i] = false;
  } 
  return _seqStates;
}

RTPSequencesState RTPScene::getSequencesState(){
  RTPSequencesState seqsState;
  for(size_t i=0; i<SequencerScene.size(); i++){
    seqsState.sequenceState[i].state = SequencerScene[i].isCurrentSequenceEnabled();
    seqsState.sequenceState[i].color = SequencerScene[i].getColor();
  }
  return seqsState;
}

int RTPScene::getSequenceType(int sequenceIndex)  {
  return SequencerScene[sequenceIndex].getType();
}

void RTPScene::selectParameterInSequece(int parameterIndex){
  SequencerScene[_selectedSequence].selectParameter(parameterIndex);
}

void RTPScene::incselectParameterInSequece(){
  SequencerScene[_selectedSequence].increaseParameterValue();
}

void RTPScene::decselectParameterInSequece(){
  SequencerScene[_selectedSequence].decreaseParameterValue();
}

void RTPScene::incselectPageInSequence(){
  SequencerScene[_selectedSequence].increasePage();
}

void RTPScene::decselectPageInSequence(){
  SequencerScene[_selectedSequence].decreasePage();
}

int RTPScene::getSelectedSequenceCurrentPosition(){
  return SequencerScene[_selectedSequence].getCurrentSequencePosition();
}

int RTPScene::getSelectedParameterInSequeceValue(){
  return SequencerScene[_selectedSequence].getParameterValue();
}

void RTPScene::toggleSequence(int sequenceIndex){
  SequencerScene[sequenceIndex].enableSequence(!SequencerScene[sequenceIndex].isCurrentSequenceEnabled());
}

void RTPScene::toggleNoteInSequence(int position){
  SequencerScene[_selectedSequence].editNoteInSequence(position,
  !SequencerScene[_selectedSequence].getNoteStateInSequence(position));
}

void RTPScene::editNoteInCurrentPosition(ControlCommand command){
  SequencerScene[_selectedSequence].editNoteInCurrentPosition(command);
}

RTPSequenceNoteStates RTPScene::getSequenceNoteStates(){
  for(size_t i=0; i<SEQ_BLOCK_SIZE; i++)
    _seqStates.val[i] = SequencerScene[_selectedSequence].getNoteStateInSequence(i);
  return _seqStates;
}

int RTPScene::getSequenceColor(){
  return SequencerScene[_selectedSequence].getColor();
}

void RTPScene::dumpSequencesToJson(){
  String fileString = "";
  for(size_t i=0; i<SequencerScene.size(); i++)
    fileString += SequencerScene[i].dumpSequenceToJson();
    fileString += "\t\n";
  writeToFile("sequences.json", fileString);
}
