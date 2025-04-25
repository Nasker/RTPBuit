#include "RTPScene.h"
#include "DrumSequence.hpp"
#include "BassSequence.hpp"
#include "MonoSequence.hpp"
#include "PolySequence.hpp"
#include "ControlSequence.hpp"
#include "HarmonySequence.hpp"
#include "RTPSDManager.hpp"
#include <cstdint>

uint8_t types[N_SCENES][SCENE_BLOCK_SIZE] = {
  {HARMONY_TRACK, CONTROL_TRACK, CONTROL_TRACK, CONTROL_TRACK, CONTROL_TRACK, CONTROL_TRACK, CONTROL_TRACK, CONTROL_TRACK, CONTROL_TRACK, CONTROL_TRACK, CONTROL_TRACK, CONTROL_TRACK, CONTROL_TRACK, CONTROL_TRACK, CONTROL_TRACK, CONTROL_TRACK},
  {DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART, DRUM_PART},
  {BASS_SYNTH, BASS_SYNTH, BASS_SYNTH, BASS_SYNTH, MONO_SYNTH, MONO_SYNTH, MONO_SYNTH, MONO_SYNTH, MONO_SYNTH, MONO_SYNTH, POLY_SYNTH, POLY_SYNTH, POLY_SYNTH, POLY_SYNTH, POLY_SYNTH, POLY_SYNTH},
};

uint8_t midiChannels[N_SCENES][SCENE_BLOCK_SIZE] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
  {1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 12, 13, 14, 15, 16},
};

RTPScene::RTPScene(String name, uint8_t NSequences, uint8_t scene, NotesPlayer& notesPlayer, MusicManager& musicManager) 
  : _name(name), _NSequences(NSequences), _selectedSequence(0), _notesPlayer(notesPlayer), _musicManager(musicManager) {
  for (uint8_t i = 0; i < _NSequences; i++) {
    uint8_t baseNote = (types[scene][i] == DRUM_PART) ? 36 + i : 60;
        std::unique_ptr<RTPEventNoteSequence> sequence;
        switch (types[scene][i]) {
            case DRUM_PART:
                sequence = std::make_unique<DrumSequence>(midiChannels[scene][i], SEQ_BLOCK_SIZE * N_PAGES, types[scene][i], baseNote, _notesPlayer, _musicManager);
                break;
            case BASS_SYNTH:
                sequence = std::make_unique<BassSequence>(midiChannels[scene][i], SEQ_BLOCK_SIZE * N_PAGES, types[scene][i], baseNote, _notesPlayer, _musicManager);
                break;
            case MONO_SYNTH:
                sequence = std::make_unique<MonoSequence>(midiChannels[scene][i], SEQ_BLOCK_SIZE * N_PAGES, types[scene][i], baseNote, _notesPlayer, _musicManager);
                break;
            case POLY_SYNTH:
                sequence = std::make_unique<PolySequence>(midiChannels[scene][i], SEQ_BLOCK_SIZE * N_PAGES, types[scene][i], baseNote, _notesPlayer, _musicManager);
                break;
            case CONTROL_TRACK:
                sequence = std::make_unique<ControlSequence>(midiChannels[scene][i], SEQ_BLOCK_SIZE * N_PAGES, types[scene][i], baseNote, _notesPlayer, _musicManager);
                break;
            case HARMONY_TRACK:
                sequence = std::make_unique<HarmonySequence>(midiChannels[scene][i], SEQ_BLOCK_SIZE * N_PAGES, types[scene][i], baseNote, _notesPlayer, _musicManager);
                break;
            default:
                continue; // Skip if type is unknown
        }
    SequencerScene.push_back(move(sequence));
  }
}

void RTPScene::playScene() {
  for (auto& sequence : SequencerScene)
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
    seqsState.sequenceState[i].color = SequencerScene[i]->getColor();
  }
  return seqsState;
}

SequenceSettings RTPScene::getSelectedSequenceSettings() {
  SequenceSettings settings;
  settings.type = SequencerScene[_selectedSequence]->getType();
  settings.lenght = SequencerScene[_selectedSequence]->getSequenceSize();
  settings.color = SequencerScene[_selectedSequence]->getColor();
  settings.midiChannel = SequencerScene[_selectedSequence]->getMidiChannel();
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
  return SequencerScene[_selectedSequence]->getColor();
}

void RTPScene::dumpSequencesToJson() {
  String fileString;
  for (const auto& sequence : SequencerScene)
    fileString += sequence->dumpSequenceToJson() + "\t\n";
  writeToFile("sequences.json", fileString);
}

// For persistence manager - Non-const version
RTPEventNoteSequence* RTPScene::getSequence(uint8_t index) {
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
