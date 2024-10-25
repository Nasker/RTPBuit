#include "RTPSequencer.h"

RTPSequencer::RTPSequencer(int NScenes, MusicManager& musicManager)
    : _musicManager(musicManager), _NScenes(NScenes), _selectedScene(0) {
    Serial.println("CREATING RTPSequencer");
    for (int i = 0; i < _NScenes; i++) {
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
        // print which scene is playing in a line using Serial.printf formatting
        Serial.printf("Playing scene %d\n", i);
        Sequencer[i]->playScene(); // Use -> to access members
        Serial.printf("Forwarding scene %d\n", i);
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

int RTPSequencer::getSelectedSequencePosition() {
    return Sequencer[_selectedScene]->getSelectedSequenceCurrentPosition();
}

int RTPSequencer::getSelectedSequencePageOffset() {
    return Sequencer[_selectedScene]->getSelectedSequencePageOffset();
}

int RTPSequencer::getSelectedSequencePage() {
    return Sequencer[_selectedScene]->getSelectedSequencePage();
}

void RTPSequencer::selectScene(int scene) {
    _selectedScene = scene;
}

void RTPSequencer::increaseSelectedScene() {
    _selectedScene++;
    if (_selectedScene >= _NScenes - 1)
        _selectedScene = _NScenes - 1;
}

void RTPSequencer::decreaseSelectedScene() {
    _selectedScene--;
    if (_selectedScene <= 0)
        _selectedScene = 0;
}

int RTPSequencer::getSelectScene() {
    return _selectedScene;
}

void RTPSequencer::addScene(RTPScene* scene) { // Change to pointer type
    Sequencer.push_back(scene);
}

void RTPSequencer::removeScene(int scene) {
    // Implement if needed
}

void RTPSequencer::toggleNoteInSceneInSelectedSequence(int position) {
    Sequencer[_selectedScene]->toggleNoteInSequence(position);
}

void RTPSequencer::toggleSequence(int sequenceIndex) {
    Sequencer[_selectedScene]->toggleSequence(sequenceIndex);
}

RTPSequencesState RTPSequencer::getSequencesState() {
    return Sequencer[_selectedScene]->getSequencesState();
}

void RTPSequencer::selectSequence(int sequenceIndex) {
    Sequencer[_selectedScene]->setSelectedSequence(sequenceIndex);
}

int RTPSequencer::getSelectedSequence() {
    return Sequencer[_selectedScene]->getSelectedSequence();
}

void RTPSequencer::selectParameterInSequence(int parameterIndex) {
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

int RTPSequencer::getSelectedSequenceColor() {
    return Sequencer[_selectedScene]->getSequenceColor();
}

void RTPSequencer::dumpSequencesToJson() {
    for (size_t i = 0; i < Sequencer.size(); i++)
        Sequencer[i]->dumpSequencesToJson();
}
