#pragma once

#include <RTPScene.h>
#include "NotesPlayer.hpp"
#include "Structs.h"
#include <vector>
#include "MusicManager.hpp"
using namespace std;

class RTPSequencer{
  vector<RTPScene*> Sequencer; // Change to pointer type
  int _NScenes;
  int _selectedScene;
  NotesPlayer _notesPlayer;
  MusicManager& _musicManager;

public:
  RTPSequencer(int NScenes, MusicManager& musicManager);
  ~RTPSequencer(); // Destructor for cleaning up
  void playAndMoveSequencer();
  void stopAndCleanSequencer();
  void pauseSequencer();
  int getSelectedSequencePosition();
  int getSelectedSequencePageOffset();
  int getSelectedSequencePage();
  bool isSelectedSequenceRecording();
  void toggleSelectedSequenceRecording();
  SequenceSettings getSelectedSequenceSettings();
  void selectScene(int scene);
  void increaseSelectedScene();
  void decreaseSelectedScene();
  int getSelectScene();
  int getSelectedSequence();
  int getSelectedSequenceMidiChannel();
  void addScene(RTPScene* scene); // Change to pointer type
  void removeScene(int scene);
  void selectParameterInSequence(int parameterIndex);
  void incSelectParameterInSequence();
  void decSelectParameterInSequence();
  int getSelectedParameterInSequenceValue();
  String getSelectedParameterInSequenceName();
  void toggleSequence(int sequenceIndex);
  RTPSequencesState getSequencesState();
  void selectSequence(int sequenceIndex);
  RTPSequenceNoteStates getSelectedSequenceNoteStates();
  void toggleNoteInSceneInSelectedSequence(int position);
  void nudgePageInSelectedSequence(ControlCommand command);
  void editNoteInCurrentPosition(ControlCommand command);
  int getSelectedSequenceColor();
  void dumpSequencesToJson();
  
  // For persistence manager
  int getNumScenes() const { return _NScenes; }
  RTPScene* getScene(int index) const { return (index >= 0 && index < Sequencer.size()) ? Sequencer[index] : nullptr; }
};