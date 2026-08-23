#pragma once

#include <cstdint>
#include "Interfaces/IMidiOutput.hpp"
#include "Interfaces/ISequencer.hpp"
#include <RTPScene.h>
#include "NotesPlayer.hpp"
#include "Structs.h"
#include <vector>
#include "MusicManager.hpp"
using namespace std;

class RTPSequencer : public ISequencer {
  vector<RTPScene*> Sequencer;
  uint8_t _NScenes;
  uint8_t _selectedScene;
  NotesPlayer _notesPlayer;
  MusicManager& _musicManager;
  bool _isPlaying = false;

public:
  RTPSequencer(uint8_t NScenes, MusicManager& musicManager);
  ~RTPSequencer();

  // ---- original API (unchanged) ----
  void playAndMoveSequencer();
  void stopAndCleanSequencer();
  void pauseSequencer();
  bool isPlaying() const override;
  uint16_t getSelectedSequencePosition();
  uint16_t getSelectedSequencePageOffset();
  uint8_t getSelectedSequencePage();
  bool isSelectedSequenceRecording();
  void toggleSelectedSequenceRecording();
  SequenceSettings getSelectedSequenceSettings();
  void selectScene(uint8_t scene) override;
  void increaseSelectedScene();
  void decreaseSelectedScene();
  uint8_t getSelectScene();
  uint8_t getSelectedSequence();
  uint8_t getSelectedSequenceMidiChannel();
  String getSelectedSequenceTypeName();
  void addScene(RTPScene* scene);
  void addDynamicScene() override;
  void removeCurrentScene() override;
  void removeScene(uint8_t scene);
  void selectParameterInSequence(uint8_t parameterIndex);
  void incSelectParameterInSequence();
  void decSelectParameterInSequence();
  int getSelectedParameterInSequenceValue();
  String getSelectedParameterInSequenceName();
  void toggleSequence(uint8_t sequenceIndex) override;
  void toggleAllSequencesInScene();
  RTPSequencesState getSequencesState();
  void selectSequence(uint8_t sequenceIndex) override;
  RTPSequenceNoteStates getSelectedSequenceNoteStates();
  void toggleNoteInSceneInSelectedSequence(uint16_t position);
  bool nudgePageInSelectedSequence(ControlCommand command);
  void editNoteInCurrentPosition(ControlCommand command);
  uint16_t getSelectedSequenceSize();
  uint32_t getSelectedSequenceColor();
  void dumpSequencesToJson();
  void playLiveNoteOn(uint8_t rootNote, uint8_t velocity, uint8_t chordType) override;
  void playLiveNoteOff(uint8_t rootNote, uint8_t chordType) override;
  void handleLiveThreeAxis(ControlCommand command);
  void handleLiveSequencerTick();
  void handleLiveHalfTick();
  uint8_t getLiveVelocity() override;

  void setMidiOutput(IMidiOutput* midiOutput);
  RTPEventNoteSequence* getActiveSequence();

  // For persistence manager
  int getNumScenes() const override { return (int)Sequencer.size(); }
  RTPScene* getScene(int index) const { return (index >= 0 && index < (int)Sequencer.size()) ? Sequencer[index] : nullptr; }
  MusicManager& getMusicManager() { return _musicManager; }

  // ---- ISequencer interface — wrappers / stubs ----
  void play() override          { playAndMoveSequencer(); }
  void stop() override          { stopAndCleanSequencer(); }
  void pause() override         { pauseSequencer(); }

  void nextScene() override     { increaseSelectedScene(); }
  void previousScene() override { decreaseSelectedScene(); }
  uint8_t getCurrentScene() const override { return _selectedScene; }
  uint8_t getSceneCount() const override   { return (uint8_t)Sequencer.size(); }

  uint8_t getCurrentSequence() const override { return const_cast<RTPSequencer*>(this)->getSelectedSequence(); }
  void toggleAllSequences() override          { toggleAllSequencesInScene(); }

  void toggleNote(uint16_t position) override { toggleNoteInSceneInSelectedSequence(position); }
  void editNote(uint16_t position, bool state) override;
  void editNote(uint16_t position, uint8_t note, uint8_t velocity) override;

  void handleLiveThreeAxis(int left, int center, int right) override;
  void handleLiveTick() override   { handleLiveSequencerTick(); }
  void handleLiveFineTick() override { handleLiveHalfTick(); }

  bool isRecording() const override { return const_cast<RTPSequencer*>(this)->isSelectedSequenceRecording(); }
  void toggleRecording() override   { toggleSelectedSequenceRecording(); }

  uint16_t getCurrentPosition() const override { return const_cast<RTPSequencer*>(this)->getSelectedSequencePosition(); }
  uint16_t getSequenceLength() const override  { return const_cast<RTPSequencer*>(this)->getSelectedSequenceSize(); }
  uint8_t  getSequenceType() const override;
  uint8_t  getMidiChannel() const override     { return const_cast<RTPSequencer*>(this)->getSelectedSequenceMidiChannel(); }
  String   getSequenceTypeName() const override { return const_cast<RTPSequencer*>(this)->getSelectedSequenceTypeName(); }
  uint32_t getSequenceColor() const override   { return const_cast<RTPSequencer*>(this)->getSelectedSequenceColor(); }
  RTPSequenceNoteStates getNoteStates() const override { return const_cast<RTPSequencer*>(this)->getSelectedSequenceNoteStates(); }
  RTPSequencesState getSequencesState() const override { return const_cast<RTPSequencer*>(this)->getSequencesState(); }

  void selectParameter(uint8_t parameter) override { selectParameterInSequence(parameter); }
  void increaseParameter() override { incSelectParameterInSequence(); }
  void decreaseParameter() override { decSelectParameterInSequence(); }
  int  getParameterValue() const override { return const_cast<RTPSequencer*>(this)->getSelectedParameterInSequenceValue(); }
  String getParameterName() const override { return const_cast<RTPSequencer*>(this)->getSelectedParameterInSequenceName(); }

  void nextPage() override;
  void previousPage() override;
  uint8_t getCurrentPage() const override { return const_cast<RTPSequencer*>(this)->getSelectedSequencePage(); }
  uint16_t getPageOffset() const override { return const_cast<RTPSequencer*>(this)->getSelectedSequencePageOffset(); }

  void saveToStorage(const String& filename) override { dumpSequencesToJson(); }
  void loadFromStorage(const String&) override {}
  bool fileExists(const String&) const override { return false; }
};