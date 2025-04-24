#pragma once

#include <cstdint>
#include "RTPEventNoteSequence.h"
#include "Structs.h"
#include <vector>
#include <memory>
using namespace std;

class RTPScene{
  vector<unique_ptr<RTPEventNoteSequence>> SequencerScene;
  String _name;
  uint8_t _NSequences;
  uint8_t _selectedSequence;
  NotesPlayer& _notesPlayer; 
  MusicManager& _musicManager;
  RTPSequenceNoteStates _seqStates;
public:
  RTPScene(String name, uint8_t NSequences, uint8_t scene, NotesPlayer& notesPlayer, MusicManager& musicManager);
  void playScene();
  void fordwardScene();
  void backwardScene();
  void resetScene();
  void setSelectedSequence(uint8_t selectedSequence);
  uint8_t getSelectedSequence();
  uint16_t getSelectedSequencePageOffset();
  uint8_t getSelectedSequencePage();
  uint8_t getSelectedSequenceMidiChannel();
  uint8_t getSize();
  bool isSelectedSequenceRecording();
  void toggleSelectedSequenceRecording();
  RTPSequenceNoteStates getSelectedSequenceNoteStates();
  RTPSequencesState getSequencesState();
  SequenceSettings getSelectedSequenceSettings();
  void selectParameterInSequence(uint8_t parameterIndex);
  void incselectParameterInSequence();
  void decselectParameterInSequence();
  int getSelectedParameterInSequenceValue();
  String getSelectedParameterInSequenceName();
  void incselectPageInSequence();
  void decselectPageInSequence();
  uint16_t getSelectedSequenceCurrentPosition();
  uint8_t getSequenceType(uint8_t sequenceIndex);
  void toggleSequence(uint8_t sequenceIndex);
  void toggleNoteInSequence(uint16_t position);
  void editNoteInCurrentPosition(ControlCommand command);
  RTPSequenceNoteStates getSequenceNoteStates();
  uint32_t getSequenceColor();
  void dumpSequencesToJson();
  
  // For persistence manager
  RTPEventNoteSequence* getSequence(uint8_t index);
  const RTPEventNoteSequence* getSequence(int index) const;
};
