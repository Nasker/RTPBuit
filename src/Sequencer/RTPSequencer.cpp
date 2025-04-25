#include "RTPSequencer.h"
#include "BuitPersistenceManager.hpp"
#include <cstdint>

RTPSequencer::RTPSequencer(uint8_t NScenes, MusicManager& musicManager)
    : _musicManager(musicManager), _NScenes(NScenes), _selectedScene(0) {
    Serial.println("CREATING RTPSequencer");
    for (uint8_t i = 0; i < _NScenes; i++) {
        RTPScene* scene = new RTPScene("Scene", SCENE_BLOCK_SIZE, i, _notesPlayer, _musicManager);
        Sequencer.push_back(scene);
    }
}

RTPSequencer::~RTPSequencer() {
    for (size_t i = 0; i < Sequencer.size(); i++) {
        delete Sequencer[i]; // Free the allocated memory
    }
    Sequencer.clear(); // Clear the vector
}

void RTPSequencer::playAndMoveSequencer() {
    for (size_t i = 0; i < Sequencer.size(); i++) { 
        Sequencer[i]->playScene();
        _notesPlayer.playNotes();
        Sequencer[i]->fordwardScene();
        _notesPlayer.decreaseTimeToLive();
    }
}

void RTPSequencer::stopAndCleanSequencer() {
    for (size_t i = 0; i < Sequencer.size(); i++)
        Sequencer[i]->resetScene();
    _notesPlayer.killAllNotes();
}

void RTPSequencer::pauseSequencer() {
    _notesPlayer.killAllNotes();
}

uint16_t RTPSequencer::getSelectedSequencePosition() {
    return Sequencer[_selectedScene]->getSelectedSequenceCurrentPosition();
}

uint16_t RTPSequencer::getSelectedSequencePageOffset() {
    return Sequencer[_selectedScene]->getSelectedSequencePageOffset();
}

uint8_t RTPSequencer::getSelectedSequencePage() {
    return Sequencer[_selectedScene]->getSelectedSequencePage();
}

bool RTPSequencer::isSelectedSequenceRecording() {
    return Sequencer[_selectedScene]->isSelectedSequenceRecording();
}

void RTPSequencer::toggleSelectedSequenceRecording() {
    Sequencer[_selectedScene]->toggleSelectedSequenceRecording();
}

void RTPSequencer::selectScene(uint8_t scene) {
    _selectedScene = scene;
}

void RTPSequencer::increaseSelectedScene() {
    if (_selectedScene < _NScenes - 1)
        _selectedScene++;
}

void RTPSequencer::decreaseSelectedScene() {
    if (_selectedScene > 0)
        _selectedScene--;
}

uint8_t RTPSequencer::getSelectScene() {
    return _selectedScene;
}

void RTPSequencer::addScene(RTPScene* scene) { // Change to pointer type
    Sequencer.push_back(scene);
}

void RTPSequencer::removeScene(uint8_t scene) {
    // Implement if needed
}

void RTPSequencer::toggleNoteInSceneInSelectedSequence(uint16_t position) {
    Sequencer[_selectedScene]->toggleNoteInSequence(position);
}

void RTPSequencer::toggleSequence(uint8_t sequenceIndex) {
    Sequencer[_selectedScene]->toggleSequence(sequenceIndex);
}

RTPSequencesState RTPSequencer::getSequencesState() {
    return Sequencer[_selectedScene]->getSequencesState();
}

void RTPSequencer::selectSequence(uint8_t sequenceIndex) {
    Sequencer[_selectedScene]->setSelectedSequence(sequenceIndex);
}

uint8_t RTPSequencer::getSelectedSequence() {
    return Sequencer[_selectedScene]->getSelectedSequence();
}

uint8_t RTPSequencer::getSelectedSequenceMidiChannel(){
    return Sequencer[_selectedScene]->getSelectedSequenceMidiChannel();
}

String RTPSequencer::getSelectedSequenceTypeName() {
    // Get the sequence type as an integer
    SequenceSettings settings = getSelectedSequenceSettings();
    int typeValue = settings.type;
    
    // Map the type value to a meaningful name
    switch (typeValue) {
        case 0:
            return "Drum";
        case 1:
            return "Bass";
        case 2:
            return "Lead";
        case 3:
            return "Polyphonic";
        case 4:
            return "Automation";
        case 5:
            return "Harmony";
        default:
            return "Type " + String(typeValue);
    }
}

void RTPSequencer::selectParameterInSequence(uint8_t parameterIndex) {
    Sequencer[_selectedScene]->selectParameterInSequence(parameterIndex);
}

int RTPSequencer::getSelectedParameterInSequenceValue() {
    return Sequencer[_selectedScene]->getSelectedParameterInSequenceValue();
}

String RTPSequencer::getSelectedParameterInSequenceName() {
    return Sequencer[_selectedScene]->getSelectedParameterInSequenceName();
}

void RTPSequencer::incSelectParameterInSequence() {
    Sequencer[_selectedScene]->incselectParameterInSequence();
}

void RTPSequencer::decSelectParameterInSequence() {
    Sequencer[_selectedScene]->decselectParameterInSequence();
}

SequenceSettings RTPSequencer::getSelectedSequenceSettings() {
    return Sequencer[_selectedScene]->getSelectedSequenceSettings(); 
}

RTPSequenceNoteStates RTPSequencer::getSelectedSequenceNoteStates() {
    return Sequencer[_selectedScene]->getSequenceNoteStates();
}

void RTPSequencer::nudgePageInSelectedSequence(ControlCommand command) {
    switch (command.commandType) {
        case ROTATING_LEFT:
            Sequencer[_selectedScene]->decselectPageInSequence();
            return;
        case ROTATING_RIGHT:
            Sequencer[_selectedScene]->incselectPageInSequence();
            return;
    }
}

void RTPSequencer::editNoteInCurrentPosition(ControlCommand command) {
    Sequencer[_selectedScene]->editNoteInCurrentPosition(command);
}

uint32_t RTPSequencer::getSelectedSequenceColor() {
    return Sequencer[_selectedScene]->getSequenceColor();
}

void RTPSequencer::dumpSequencesToJson() {
    // Create persistence manager and use it to save the sequences
    BuitPersistenceManager persistenceManager;
    persistenceManager.saveSequencerToFile(*this);
    
    Serial.println("Saved sequences using BuitPersistenceManager");
}
