#pragma once

#include <RTPScene.h>
#include "NotesPlayer.hpp"
#include "Structs.h"
#include <vector>
#include "MusicManager.hpp"
using namespace std;

class RTPSequencer{
  vector<RTPScene> Sequencer;
  int _NScenes;
  int _selectedScene;
  NotesPlayer _notesPlayer;
  MusicManager& _musicManager;

public:
  RTPSequencer(int NScenes, MusicManager& musicManager);
  //void connectMainUnit(RTPMainUnit* mainUnit);
  void playAndMoveSequencer();
  void stopAndCleanSequencer();
  void pauseSequencer();
  int getSelectedSequencePosition();
  int getSelectedSequencePageOffset();
  int getSelectedSequencePage();
  SequenceSettings getSelectedSequenceSettings();
  void selectScene(int scene);
  void increaseSelectedScene();
  void decreaseSelectedScene();
  int getSelectScene();
  int getSelectedSequence();
  void addScene(RTPScene scene);
  void removeScene(int scene);
  void selectParameterInSequece(int parameterIndex);
  void incselectParameterInSequece();
  void decselectParameterInSequece();
  int getSelectedParameterInSequeceValue();
  void toggleSequence(int sequenceIndex);
  RTPSequencesState getSequencesState();
  void selectSequence(int sequenceIndex);
  RTPSequenceNoteStates getSelectedSequenceNoteStates();
  void toggleNoteInSceneInSelectedSequence(int position);
  void nudgePageInSelectedSequence(ControlCommand command);
  void editNoteInCurrentPosition(ControlCommand command);
  int getSelectedSequenceColor();
  void dumpSequencesToJson();
};
