#pragma once

#include "RTPEventNoteSequence.h"
#include "Structs.h"
#include <vector>
#include <memory>
using namespace std;

class RTPScene{
  vector<unique_ptr<RTPEventNoteSequence>> SequencerScene;
  String _name;
  int _NSequences;
  int _selectedSequence;
  NotesPlayer& _notesPlayer; 
  MusicManager& _musicManager;
  RTPSequenceNoteStates _seqStates;
public:
  RTPScene(String name, int NSequences, int scene, NotesPlayer& notesPlayer, MusicManager& musicManager);
  void playScene();
  void fordwardScene();
  void backwardScene();
  void resetScene();
  void setSelectedSequence(int selectedSequence);
  int getSelectedSequence();
  int getSelectedSequencePageOffset();
  int getSelectedSequencePage();
  int getSelectedSequenceMidiChannel();
  int getSize();
  RTPSequenceNoteStates getSelectedSequenceNoteStates();
  RTPSequencesState getSequencesState();
  SequenceSettings getSelectedSequenceSettings();
  void selectParameterInSequence(int parameterIndex);
  void incselectParameterInSequence();
  void decselectParameterInSequence();
  int getSelectedParameterInSequenceValue();
  String getSelectedParameterInSequenceName();
  void incselectPageInSequence();
  void decselectPageInSequence();
  int getSelectedSequenceCurrentPosition();
  int getSequenceType(int sequenceIndex);
  void toggleSequence(int sequenceIndex);
  void toggleNoteInSequence(int position);
  void editNoteInCurrentPosition(ControlCommand command);
  RTPSequenceNoteStates getSequenceNoteStates();
  int getSequenceColor();
  void dumpSequencesToJson();
};
