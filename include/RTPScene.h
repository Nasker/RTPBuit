#pragma once

#include "RTPEventNoteSequence.h"
#include "Structs.h"
#include <vector>
using namespace std;

class RTPScene{
  vector<RTPEventNoteSequence> SequencerScene;
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
  int getSize();
  RTPSequenceNoteStates getSelectedSequenceNoteStates();
  RTPSequencesState getSequencesState();
  void selectParameterInSequece(int parameterIndex);
  void incselectParameterInSequece();
  void decselectParameterInSequece();
  void incselectPageInSequence();
  void decselectPageInSequence();
  int getSelectedSequenceCurrentPosition();
  int getSelectedParameterInSequeceValue();
  int getSequenceType(int sequenceIndex);
  void toggleSequence(int sequenceIndex);
  void toggleNoteInSequence(int position);
  void editNoteInCurrentPosition(ControlCommand command);
  RTPSequenceNoteStates getSequenceNoteStates();
  int getSequenceColor();
  void dumpSequencesToJson();
};
