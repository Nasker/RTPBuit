#include "BuitDevicesManager.hpp"
#include "Sequencer/RTPEventNoteSequence.h"

BuitDevicesManager::BuitDevicesManager(RTPNeoTrellis& nT, RTPSequencer& seq):
_neoTrellis(nT),
_sequencer(seq){}

void BuitDevicesManager::initSetup(){
    _oled.init();
    initBuitSD();
    introAnimations();

}

void BuitDevicesManager::introAnimations(){
    int x = 128;
    String text = "I'm RTP's BUIT!!";
    for (int i = 0; i < 55; i++) {
        _oled.introAnimation(x, text);
        _neoTrellis.introAnimation();
    }
    _oled.setAfterIntro();
}

void BuitDevicesManager::printToScreen(String firstLine, String secondLine, String thirdLine){
    _oled.printToScreen(firstLine, secondLine, thirdLine);
}

void BuitDevicesManager::printToScreen(ControlCommand command){
    _oled.printToScreen(command);
}

void BuitDevicesManager::writeSequenceToNeoTrellis(RTPSequenceNoteStates sequenceStates, int color){
    _neoTrellis.writeSequenceStates(sequenceStates, color);
}

void BuitDevicesManager::writeSceneToNeoTrellis(RTPSequencesState sequencesState){
    _neoTrellis.writeSceneStates(sequencesState);
}

void BuitDevicesManager::writeTransportPage(){
    // Light up transport control buttons on trellis
    // Layout: [Play][Stop][Rec][Tap][Mode] on first row
    // Colors: Play=Green, Stop=Blue, Rec=Red, Tap=Yellow, Mode=White
    _neoTrellis.clearAllButtons();
    
    // Play button (0) - Green if playing, dim green if stopped
    if (_sequencer.isPlaying()) {
        _neoTrellis.setButtonColor(0, _neoTrellis.colorGreen());
    } else {
        _neoTrellis.setButtonColor(0, _neoTrellis.colorDim(_neoTrellis.colorGreen(), 128));
    }
    
    // Stop button (1) - Blue when stopped, dim when playing
    if (!_sequencer.isPlaying()) {
        _neoTrellis.setButtonColor(1, _neoTrellis.colorBlue());
    } else {
        _neoTrellis.setButtonColor(1, _neoTrellis.colorDim(_neoTrellis.colorBlue(), 128));
    }
    
    // Rec button (2) - Red when recording, yellow when waiting
    if (isSelectedSequenceRecording()) {
        if (isSelectedSequenceWaiting()) {
            _neoTrellis.setButtonColor(2, _neoTrellis.colorYellow()); // Waiting
        } else {
            _neoTrellis.setButtonColor(2, _neoTrellis.colorRed()); // Recording
        }
    } else {
        _neoTrellis.setButtonColor(2, _neoTrellis.colorDim(_neoTrellis.colorRed(), 64));
    }
    
    // Tap button (3) - Yellow
    _neoTrellis.setButtonColor(3, _neoTrellis.colorYellow());
    
    // Mode button (4) - White for internal, dim white for external
    if (getSyncMode() == rtp::SyncMode::Internal) {
        _neoTrellis.setButtonColor(4, _neoTrellis.colorWhite());
    } else {
        _neoTrellis.setButtonColor(4, _neoTrellis.colorDim(_neoTrellis.colorWhite(), 128));
    }
    
    _neoTrellis.show();
}

void BuitDevicesManager::editScene(ControlCommand command){
    _sequencer.toggleSequence(command.value);
    writeSceneToNeoTrellis(_sequencer.getSequencesState());
}

void BuitDevicesManager::editSequence(ControlCommand command){
    _sequencer.toggleNoteInSceneInSelectedSequence(command.value);
}

void BuitDevicesManager::editCurrentNote(ControlCommand command){
    _sequencer.editNoteInCurrentPosition(command);
}



void BuitDevicesManager::displayCursorInSequence(ControlCommand command){
    int cursorPos = _sequencer.getSelectedSequencePosition() - _sequencer.getSelectedSequencePageOffset();
    if (cursorPos >= 0 && cursorPos < SEQ_BLOCK_SIZE){
        _neoTrellis.writeSequenceStates(_sequencer.getSelectedSequenceNoteStates(), _sequencer.getSelectedSequenceColor(), false);
        _neoTrellis.moveCursor(cursorPos);
    }
    else if (cursorPos == SEQ_BLOCK_SIZE){
        _neoTrellis.writeSequenceStates(_sequencer.getSelectedSequenceNoteStates(), _sequencer.getSelectedSequenceColor());
    }    
}   

void BuitDevicesManager::changeScene(ControlCommand command){
    switch(command.commandType){
        case ROTATING_RIGHT:
            _sequencer.increaseSelectedScene();
            break;
        case ROTATING_LEFT:
            _sequencer.decreaseSelectedScene();
            break;
    }
    presentScene();
}

void  BuitDevicesManager::nudgePage(ControlCommand command){
    _sequencer.nudgePageInSelectedSequence(command);
    // Call showSequence to ensure consistent display with recording status
    showSequence();
}

void BuitDevicesManager::selectParameter(ControlCommand command){
    _sequencer.selectParameterInSequence(command.value);
}

void BuitDevicesManager::rotateParameter(ControlCommand command){
    switch(command.commandType){
        case ROTATING_RIGHT:
            _sequencer.incSelectParameterInSequence();
            break;
        case ROTATING_LEFT:
            _sequencer.decSelectParameterInSequence();
            break;
    }
}

void BuitDevicesManager::presentScene(){
    // Determine scene display state based on sequencer playing status
    SequenceDisplayState state = _sequencer.isPlaying() ? SequenceDisplayState::Playing : SequenceDisplayState::Stopped;
    
    _oled.printToScreen(
        "Scene",
        "Scene " + String(_sequencer.getSelectScene() + 1),
        "",
        "",
        state,
        false  // No blink needed for play/stop
    );
    writeSceneToNeoTrellis(_sequencer.getSequencesState());
}

void BuitDevicesManager::presentSequence(){
    // Call showSequence to ensure consistent display with recording status
    showSequence();
}

void BuitDevicesManager::showSequence(){
    // Get the sequence type name
    String sequenceType = _sequencer.getSelectedSequenceTypeName();
    
    // Get the sequence MIDI channel
    int midiChannel = _sequencer.getSelectedSequenceMidiChannel();
    
    // Get the current page and total pages (assuming 4 pages total)
    int currentPage = _sequencer.getSelectedSequencePage() + 1; // +1 for 1-based display
    int totalPages = 4; // Assuming 4 pages total
    
    // Determine display state
    SequenceDisplayState state = getSequenceDisplayState();
    
    // Update blink counter for waiting state (toggle every 8 calls ~ 133ms at 60fps)
    _displayBlinkCounter++;
    bool blinkState = (_displayBlinkCounter / 8) % 2 == 0;
    
    // Use the state-based display method
    _oled.printToScreen(
        sequenceType,
        "Sequence "+ String(_sequencer.getSelectedSequence()+1),
        "Page "+ String(currentPage) + " of " + String(totalPages),
        "Ch " + String(midiChannel),
        state,
        blinkState
    );
    
    writeSequenceToNeoTrellis(_sequencer.getSelectedSequenceNoteStates(), _sequencer.getSelectedSequenceColor()); 
}

void BuitDevicesManager::presentTransport(){
    String modeStr = (getSyncMode() == rtp::SyncMode::Internal) ? "INT" : "EXT";
    String playState = _sequencer.isPlaying() ? "Playing" : "Stopped";
    String bpmStr = (getSyncMode() == rtp::SyncMode::Internal) ? String(getCurrentBPM(), 0) + " BPM" : "";
    
    SequenceDisplayState state = _sequencer.isPlaying() ? SequenceDisplayState::Playing : SequenceDisplayState::Stopped;
    
    _oled.printToScreen(
        "Transport",
        playState + "  " + modeStr,
        bpmStr,
        "Scene " + String(_sequencer.getSelectScene() + 1),
        state,
        false
    );
    
    writeTransportPage();
}

void BuitDevicesManager::presentSequenceSettings(){
    printToScreen("Settings", _sequencer.getSelectedParameterInSequenceName(), String(_sequencer.getSelectedParameterInSequenceValue()));
    _neoTrellis.writeSequenceSettingsPage(_sequencer.getSelectedSequenceSettings());
}

void BuitDevicesManager::presentBuitCC(){
    printToScreen("CCs Matrix", "", "");
    _neoTrellis.writeBuitCCStates(_matrixBuitCC.getBuitCCStates(), TRANSPORT_COLOR);
}

void BuitDevicesManager::selectScene(ControlCommand command){
    _sequencer.selectScene(command.value);
}

void BuitDevicesManager::selectSequence(ControlCommand command){
    _sequencer.selectSequence(command.value);
}

void BuitDevicesManager::editBuitCC(ControlCommand command){
    _matrixBuitCC.toggleBuitCC(command.value);
    _neoTrellis.writeBuitCCStates(_matrixBuitCC.getBuitCCStates(), TRANSPORT_COLOR);
}

void BuitDevicesManager::sendBuitCC(ControlCommand command){
    _matrixBuitCC.updateAndSend(command);
}

int BuitDevicesManager::getSelectedSequenceMidichannel(){
    return _sequencer.getSelectedSequenceMidiChannel();
}

bool BuitDevicesManager::isSelectedSequenceRecording(){
    return _sequencer.isSelectedSequenceRecording();
}

bool BuitDevicesManager::isSelectedSequenceWaiting(){
    return _notesRecorder.isWaiting();
}

SequenceDisplayState BuitDevicesManager::getSequenceDisplayState(){
    if (_notesRecorder.isRecording()) {
        return SequenceDisplayState::Recording;
    } else if (_notesRecorder.isWaiting()) {
        return SequenceDisplayState::Waiting;
    } else if (_sequencer.isSelectedSequenceRecording()) {
        // Sequencer thinks it's recording but recorder is not yet active (shouldn't happen, but handle it)
        return SequenceDisplayState::Waiting;
    } else {
        return SequenceDisplayState::Playing;
    }
}

void BuitDevicesManager::toggleSelectedSequenceRecording(){
    _sequencer.toggleSelectedSequenceRecording();
    if (_sequencer.isSelectedSequenceRecording()) {
        uint16_t seqSize = _sequencer.getSelectedSequenceSize();
        uint8_t midiChannel = _sequencer.getSelectedSequenceMidiChannel();
        uint16_t currentPos = _sequencer.getSelectedSequencePosition();
        _notesRecorder.startRecording(seqSize, midiChannel, currentPos);
    } else {
        _notesRecorder.stopRecording();
        recorderDumpToSequence();
    }
    showSequence();
}

void BuitDevicesManager::recorderNoteOn(uint8_t note, uint8_t velocity) {
    if (_notesRecorder.isRecording())
        _notesRecorder.recordNoteOn(note, velocity);
}

void BuitDevicesManager::recorderNoteOff(uint8_t note) {
    if (_notesRecorder.isRecording())
        _notesRecorder.recordNoteOff(note);
}

void BuitDevicesManager::recorderAdvanceTick() {
    // Advance tick if recording OR waiting to start
    if (!_notesRecorder.isRecording() && !_notesRecorder.isWaiting()) return;
    
    _notesRecorder.advanceTick();
    
    // Check for end of one-shot recording
    if (_notesRecorder.isRecording() && _notesRecorder.isEndOfSequence()) {
        _notesRecorder.stopRecording();
        
        // Only dump if we actually recorded something
        auto notes = _notesRecorder.dumpRecordedSequence();
        if (!notes.empty()) {
            // Apply to sequence
            RTPScene* scene = _sequencer.getScene(_sequencer.getSelectScene());
            if (scene) {
                RTPEventNoteSequence* seq = scene->getSequence(_sequencer.getSelectedSequence());
                if (seq) {
                    uint16_t seqSize = _notesRecorder.getSequenceLength();
                    seq->clearSequence();
                    seq->resizeSequence(seqSize);
                    for (auto& note : notes) {
                        uint16_t pos = note.getEventRead();
                        if (pos < seqSize) {
                            seq->editNoteInSequence(pos, note.getEventNote(), note.getEventVelocity(),
                                                    note.getLength(), note.isLiteralPitch());
                            seq->editNoteInSequence(pos, true);
                        }
                    }
                }
            }
        }
        // else: empty recording, don't overwrite existing pattern
        
        // Turn off recording mode and update display
        _sequencer.toggleSelectedSequenceRecording();
        showSequence();
    }
}

void BuitDevicesManager::recorderDumpToSequence() {
    auto notes = _notesRecorder.dumpRecordedSequence();
    if (notes.empty()) return;
    RTPScene* scene = _sequencer.getScene(_sequencer.getSelectScene());
    if (!scene) return;
    RTPEventNoteSequence* seq = scene->getSequence(_sequencer.getSelectedSequence());
    if (!seq) return;
    uint16_t seqSize = _notesRecorder.getSequenceLength();
    seq->clearSequence();
    seq->resizeSequence(seqSize);
    for (auto& note : notes) {
        uint16_t pos = note.getEventRead();
        if (pos < seqSize) {
            seq->editNoteInSequence(pos, note.getEventNote(), note.getEventVelocity(),
                                    note.getLength(), note.isLiteralPitch());
            seq->editNoteInSequence(pos, true);
        }
    }
}



void BuitDevicesManager::saveSequencer(const String& fileName){
    if(_persistenceManager.saveSequencerToFile(_sequencer, fileName))
        printToScreen("Saved", fileName, "");
    else
        printToScreen("Failed to save", fileName, "");
}

void BuitDevicesManager::loadSequencer(const String& fileName){
    if(_persistenceManager.loadSequencerFromFile(_sequencer, fileName))
        printToScreen("Loaded", fileName, "");
    else
        printToScreen("Failed to load", fileName, "");
}

// Transport control methods
bool BuitDevicesManager::isInternalClock() const {
    return _clockGenerator && _clockGenerator->getMode() == rtp::SyncMode::Internal;
}

void BuitDevicesManager::transportPlay() {
    if (!_clockGenerator) return;
    _clockGenerator->start();
}

void BuitDevicesManager::transportStop() {
    if (!_clockGenerator) return;
    _clockGenerator->stop();
}

void BuitDevicesManager::transportTapTempo() {
    if (!_clockGenerator) return;
    _clockGenerator->tapTempo();
}

void BuitDevicesManager::transportToggleMode() {
    if (!_clockGenerator) return;
    _clockGenerator->toggleMode();
}

void BuitDevicesManager::transportIncrementBPM(float delta) {
    if (!_clockGenerator) return;
    _clockGenerator->incrementBPM(delta);
}

float BuitDevicesManager::getCurrentBPM() const {
    if (!_clockGenerator) return 120.0f;
    return _clockGenerator->getBPM();
}

SyncMode BuitDevicesManager::getSyncMode() const {
    if (!_clockGenerator) return rtp::SyncMode::External;
    return _clockGenerator->getMode();
}

void BuitDevicesManager::transportSetBPM(float bpm) {
    if (!_clockGenerator) return;
    _clockGenerator->setBPM(bpm);
}

void BuitDevicesManager::incrementSwing(int delta) {
    _swingAmount += delta;
    if (_swingAmount < 0) _swingAmount = 0;
    if (_swingAmount > 100) _swingAmount = 100;
}

void BuitDevicesManager::setQuantizeStrength(int strength) {
    _quantizeStrength = strength;
    if (_quantizeStrength < 0) _quantizeStrength = 0;
    if (_quantizeStrength > 100) _quantizeStrength = 100;
    _notesRecorder.setQuantizeStrength(_quantizeStrength);
}

void BuitDevicesManager::incrementQuantizeStrength(int delta) {
    setQuantizeStrength(_quantizeStrength + delta);
}

void BuitDevicesManager::incrementMasterVolume(int delta) {
    _masterVolume += delta;
    if (_masterVolume < 0) _masterVolume = 0;
    if (_masterVolume > 100) _masterVolume = 100;
    // Future: apply master volume to all output
}