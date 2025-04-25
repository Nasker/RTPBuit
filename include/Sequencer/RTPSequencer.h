#pragma once

#include <cstdint>
#include <RTPScene.h>
#include "NotesPlayer.hpp"
#include "Structs.h"
#include <vector>
#include "MusicManager.hpp"
using namespace std;

class RTPSequencer{
  vector<RTPScene*> Sequencer; // Change to pointer type
  uint8_t _NScenes;
  uint8_t _selectedScene;
  NotesPlayer _notesPlayer;
  MusicManager& _musicManager;

public:
  RTPSequencer(uint8_t NScenes, MusicManager& musicManager);
  ~RTPSequencer(); // Destructor for cleaning up
  void playAndMoveSequencer();
  void stopAndCleanSequencer();
  void pauseSequencer();
  uint16_t getSelectedSequencePosition();
  uint16_t getSelectedSequencePageOffset();
  uint8_t getSelectedSequencePage();
  bool isSelectedSequenceRecording();
  void toggleSelectedSequenceRecording();
  SequenceSettings getSelectedSequenceSettings();
  void selectScene(uint8_t scene);
  void increaseSelectedScene();
  void decreaseSelectedScene();
  uint8_t getSelectScene();
  uint8_t getSelectedSequence();
  uint8_t getSelectedSequenceMidiChannel();
  String getSelectedSequenceTypeName();
  void addScene(RTPScene* scene); // Change to pointer type
  void removeScene(uint8_t scene);
  void selectParameterInSequence(uint8_t parameterIndex);
  void incSelectParameterInSequence();
  void decSelectParameterInSequence();
  int getSelectedParameterInSequenceValue();
  String getSelectedParameterInSequenceName();
  void toggleSequence(uint8_t sequenceIndex);
  RTPSequencesState getSequencesState();
  void selectSequence(uint8_t sequenceIndex);
  RTPSequenceNoteStates getSelectedSequenceNoteStates();
  void toggleNoteInSceneInSelectedSequence(uint16_t position);
  void nudgePageInSelectedSequence(ControlCommand command);
  void editNoteInCurrentPosition(ControlCommand command);
  uint32_t getSelectedSequenceColor();
  void dumpSequencesToJson();
  
  // For persistence manager
  int getNumScenes() const { return _NScenes; }
  RTPScene* getScene(int index) const { return (index >= 0 && index < Sequencer.size()) ? Sequencer[index] : nullptr; }
};