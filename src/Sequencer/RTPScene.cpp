#include "RTPScene.h"
#include "ColorFunctions.h"
#include "DrumSequence.hpp"
#include "BassSequence.hpp"
#include "MonoSequence.hpp"
#include "PolySequence.hpp"
#include "ControlSequence.hpp"
#include "HarmonySequence.hpp"
#include "RTPSDManager.hpp"
#include <cstdint>

uint8_t types[N_SCENES][SCENE_BLOCK_SIZE] = {
  {DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART},
  {DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART},
  {DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, HARMONY_TRACK, HARMONY_TRACK, HARMONY_TRACK, HARMONY_TRACK},
  {BASS_SYNTH, BASS_SYNTH, POLY_SYNTH, POLY_SYNTH, MONO_SYNTH, MONO_SYNTH, MONO_SYNTH, MONO_SYNTH, MONO_SYNTH, MONO_SYNTH, MONO_SYNTH, BASS_SYNTH, BASS_SYNTH, MONO_SYNTH, MONO_SYNTH, POLY_SYNTH},
};

uint8_t midiChannels[N_SCENES][SCENE_BLOCK_SIZE] = {
  {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
  {11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11},
  {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 0, 0, 0, 0},
  {1, 2, 3, 4, 5, 6, 7, 8, 9, 12, 13, 1, 1, 1, 1, 1},
};

static uint8_t baseNoteForType(uint8_t type, uint8_t index) {
  if (type == DRUM_PART)  return 36 + index;
  if (type == BASS_SYNTH) return BASS_BASE_NOTE;
  if (type == MONO_SYNTH) return BASE_NOTE;
  return 60;  // POLY and others
}

static std::unique_ptr<RTPEventNoteSequence> makeSequenceByType(
    uint8_t type, uint8_t midiChannel, uint8_t baseNote,
    NotesPlayer& notesPlayer, MusicManager& musicManager) {
  switch (type) {
    case DRUM_PART:
      return std::make_unique<DrumSequence>(midiChannel, SEQ_BLOCK_SIZE * N_PAGES, type, baseNote, notesPlayer, musicManager);
    case BASS_SYNTH:
      return std::make_unique<BassSequence>(midiChannel, SEQ_BLOCK_SIZE * N_PAGES, type, baseNote, notesPlayer, musicManager);
    case MONO_SYNTH:
      return std::make_unique<MonoSequence>(midiChannel, SEQ_BLOCK_SIZE * N_PAGES, type, baseNote, notesPlayer, musicManager);
    case POLY_SYNTH:
      return std::make_unique<PolySequence>(midiChannel, SEQ_BLOCK_SIZE * N_PAGES, type, baseNote, notesPlayer, musicManager);
    case CONTROL_TRACK:
      return std::make_unique<ControlSequence>(midiChannel, SEQ_BLOCK_SIZE * N_PAGES, type, baseNote, notesPlayer, musicManager);
    case HARMONY_TRACK:
      return std::make_unique<HarmonySequence>(midiChannel, SEQ_BLOCK_SIZE * N_PAGES, type, baseNote, notesPlayer, musicManager);
    default:
      return nullptr;
  }
}

RTPScene::RTPScene(String name, uint8_t NSequences, uint8_t scene, NotesPlayer& notesPlayer, MusicManager& musicManager) 
  : _name(name), _NSequences(NSequences), _selectedSequence(0), _notesPlayer(notesPlayer), _musicManager(musicManager) {
  for (uint8_t i = 0; i < _NSequences; i++) {
    uint8_t baseNote = baseNoteForType(types[scene][i], i);
    auto sequence = makeSequenceByType(types[scene][i], midiChannels[scene][i], baseNote, _notesPlayer, _musicManager);
    if (!sequence) continue; // Skip if type is unknown
    SequencerScene.push_back(move(sequence));
  }
}

RTPScene::RTPScene(String name, uint8_t NSequences, NotesPlayer& notesPlayer, MusicManager& musicManager)
  : _name(name), _NSequences(NSequences), _selectedSequence(0), _notesPlayer(notesPlayer), _musicManager(musicManager) {
  for (uint8_t i = 0; i < _NSequences; i++) {
    auto sequence = std::make_unique<MonoSequence>(1, SEQ_BLOCK_SIZE * N_PAGES, MONO_SYNTH, 60, _notesPlayer, _musicManager);
    SequencerScene.push_back(move(sequence));
  }
}

RTPEventNoteSequence* RTPScene::recreateSequence(uint8_t index, uint8_t type) {
  if (index >= SequencerScene.size()) return nullptr;
  uint8_t channel = SequencerScene[index]->getMidiChannel();
  auto sequence = makeSequenceByType(type, channel, baseNoteForType(type, index), _notesPlayer, _musicManager);
  if (!sequence) return nullptr;
  RTPEventNoteSequence* ptr = sequence.get();
  SequencerScene[index] = move(sequence);
  return ptr;
}

void RTPScene::toggleAllSequences() {
  for (size_t i = 0; i < SequencerScene.size(); i++)
    SequencerScene[i]->enableSequence(!SequencerScene[i]->isCurrentSequenceEnabled());
}

void RTPScene::playScene() {
  for (auto& sequence : SequencerScene)
    if (sequence->isStepPulse())
      sequence->playCurrentEventNote();
}

void RTPScene::fordwardScene() {
  for (auto& sequence : SequencerScene)
    sequence->fordwardSequence();
}

void RTPScene::backwardScene() {
  for (auto& sequence : SequencerScene)
    sequence->backwardSequence();
}

void RTPScene::resetScene() {
  for (auto& sequence : SequencerScene)
    sequence->resetSequence();
}

void RTPScene::setSelectedSequence(uint8_t selectedSequence) {
  _selectedSequence = selectedSequence;
}

uint8_t RTPScene::getSelectedSequence() {
  return _selectedSequence;
}

uint16_t RTPScene::getSelectedSequencePageOffset() {
  return SequencerScene[_selectedSequence]->pageOffset();
}

uint8_t RTPScene::getSelectedSequencePage() {
  return SequencerScene[_selectedSequence]->page();
}

uint8_t RTPScene::getSelectedSequenceMidiChannel(){
  return SequencerScene[_selectedSequence]->getMidiChannel();
}

uint8_t RTPScene::getSize() {
  return SequencerScene.size();
}

uint8_t RTPScene::getSize() const {
  return SequencerScene.size();
}

bool RTPScene::isSelectedSequenceRecording() {
  return SequencerScene[_selectedSequence]->isRecording();
}

void RTPScene::toggleSelectedSequenceRecording() {
  SequencerScene[_selectedSequence]->toggleRecording();
}

RTPSequenceNoteStates RTPScene::getSelectedSequenceNoteStates() {
  for (size_t i = 0; i < SEQ_BLOCK_SIZE; i++)
    _seqStates.val[i] = (i < SequencerScene[_selectedSequence]->getSequenceSize()) ? SequencerScene[_selectedSequence]->getNoteStateInSequence(i) : false;
  return _seqStates;
}

RTPSequencesState RTPScene::getSequencesState() {
  RTPSequencesState seqsState;
  for (size_t i = 0; i < SequencerScene.size(); i++) {
    seqsState.sequenceState[i].state = SequencerScene[i]->isCurrentSequenceEnabled();
    seqsState.sequenceState[i].color = colorMapper(SequencerScene[i]->getColor());
  }
  return seqsState;
}

SequenceSettings RTPScene::getSelectedSequenceSettings() {
  SequenceSettings settings;
  settings.type = SequencerScene[_selectedSequence]->getType();
  settings.lenght = SequencerScene[_selectedSequence]->getSequenceSize() / SEQ_BLOCK_SIZE;
  settings.color = SequencerScene[_selectedSequence]->getColor();
  settings.midiChannel = SequencerScene[_selectedSequence]->getMidiChannel();
  settings.input = SequencerScene[_selectedSequence]->getInput();
  settings.port = SequencerScene[_selectedSequence]->getPort();
  return settings;
}

uint8_t RTPScene::getSequenceType(uint8_t sequenceIndex) {
  return SequencerScene[sequenceIndex]->getType();
}

void RTPScene::selectParameterInSequence(uint8_t parameterIndex) {
  SequencerScene[_selectedSequence]->selectParameter(parameterIndex);
}

int RTPScene::getSelectedParameterInSequenceValue() {
  return SequencerScene[_selectedSequence]->getParameterValue();
}

String RTPScene::getSelectedParameterInSequenceName() {
  return SequencerScene[_selectedSequence]->getParameterName();
}

void RTPScene::incselectParameterInSequence() {
  SequencerScene[_selectedSequence]->increaseParameterValue();
}

void RTPScene::decselectParameterInSequence() {
  SequencerScene[_selectedSequence]->decreaseParameterValue();
}

void RTPScene::incselectPageInSequence() {
  SequencerScene[_selectedSequence]->increasePage();
}

void RTPScene::decselectPageInSequence() {
  SequencerScene[_selectedSequence]->decreasePage();
}

uint16_t RTPScene::getSelectedSequenceCurrentPosition() {
  return SequencerScene[_selectedSequence]->getCurrentSequencePosition();
}

uint16_t RTPScene::getSelectedSequenceSize() {
  return SequencerScene[_selectedSequence]->getSequenceSize();
}

void RTPScene::toggleSequence(uint8_t sequenceIndex) {
  SequencerScene[sequenceIndex]->enableSequence(!SequencerScene[sequenceIndex]->isCurrentSequenceEnabled());
}

void RTPScene::toggleNoteInSequence(uint16_t position) {
  SequencerScene[_selectedSequence]->editNoteInSequence(position, !SequencerScene[_selectedSequence]->getNoteStateInSequence(position));
}

void RTPScene::editNoteInCurrentPosition(ControlCommand command) {
  SequencerScene[_selectedSequence]->editNoteInCurrentPosition(command);
}

RTPSequenceNoteStates RTPScene::getSequenceNoteStates() {
  for (size_t i = 0; i < SEQ_BLOCK_SIZE; i++) {
    _seqStates.val[i] = SequencerScene[_selectedSequence]->getNoteStateInSequence(i);
	_seqStates.velocity[i] = SequencerScene[_selectedSequence]->getNoteVelocityInSequence(i);
    if(_seqStates.val[i] && !_seqStates.velocity[i])
    	_seqStates.velocity[i] = 60;
  }
  return _seqStates;
}

uint32_t RTPScene::getSequenceColor() {
  return colorMapper(SequencerScene[_selectedSequence]->getColor());
}

String RTPScene::getName() const {
  return _name;
}

void RTPScene::dumpSequencesToJson() {
  String fileString;
  for (const auto& sequence : SequencerScene)
    fileString += sequence->dumpSequenceToJson() + "\t\n";
  writeToFile("sequences.json", fileString);
}

void RTPScene::setMidiOutput(IMidiOutput* midiOutput) {
  for (auto& seq : SequencerScene) {
    seq->setMidiOutput(midiOutput);
  }
}

// For persistence manager - Non-const version
RTPEventNoteSequence* RTPScene::getSequence(int index) {
  if (index >= 0 && index < SequencerScene.size()) {
    return SequencerScene[index].get();
  }
  return nullptr;
}

// For persistence manager - Const version
const RTPEventNoteSequence* RTPScene::getSequence(int index) const {
  if (index >= 0 && index < SequencerScene.size()) {
    return SequencerScene[index].get();
  }
  return nullptr;
}
