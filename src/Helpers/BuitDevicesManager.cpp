#include "BuitDevicesManager.hpp"
#include "Sequencer/RTPEventNoteSequence.h"

BuitDevicesManager::BuitDevicesManager(IDisplay& display, IButtonMatrix& trellis, RTPSequencer& seq):
_display(display),
_trellis(trellis),
_sequencer(seq),
_concreteSequencer(seq),
_settingsPresenter(display, trellis, seq, seq, _recordingManager),
_livePlayOrchestrator(display, trellis, seq, seq, _recordingManager, _livePlayManager){}

void BuitDevicesManager::initSetup(){
    // Display is initialized via DeviceManager -> DisplayManager -> IDisplay::initialize()
    initBuitSD();
    introAnimations();
    
    // Initialize composed managers
    _recordingManager.initialize();
    _livePlayManager.initialize();
}

void BuitDevicesManager::introAnimations(){
    int x = 128;
    String text = "I'm RTP's BUIT!!";
    for (int i = 0; i < 55; i++) {
        _display.showIntroFrame(x, text);
        _trellis.introAnimation();
    }
    _display.setAfterIntro();
}

void BuitDevicesManager::printToScreen(String firstLine, String secondLine, String thirdLine){
    _display.printThreeLines(firstLine, secondLine, thirdLine);
}

void BuitDevicesManager::writeSequenceToNeoTrellis(RTPSequenceNoteStates sequenceStates, int color){
    _trellis.writeSequenceStates(sequenceStates, color);
}

void BuitDevicesManager::writeSceneToNeoTrellis(RTPSequencesState sequencesState){
    _trellis.writeSceneStates(sequencesState);
}

void BuitDevicesManager::editScene(ControlCommand command){
    _sequencer.toggleSequence(command.value);
    writeSceneToNeoTrellis(_sequencer.getSequencesState());
}

void BuitDevicesManager::editSequence(ControlCommand command){
    _sequencer.toggleNote(command.value);
}

void BuitDevicesManager::editCurrentNote(ControlCommand command){
    _concreteSequencer.editNoteInCurrentPosition(command);
}



void BuitDevicesManager::displayCursorInSequence(ControlCommand command){
    int cursorPos = _sequencer.getCurrentPosition() - _sequencer.getPageOffset();
    if (cursorPos >= 0 && cursorPos < SEQ_BLOCK_SIZE){
        _trellis.writeSequenceStates(_sequencer.getNoteStates(), _sequencer.getSequenceColor(), false);
        _trellis.moveCursor(cursorPos);
    }
    else {
        _trellis.writeSequenceStates(_sequencer.getNoteStates(), _sequencer.getSequenceColor());
    }
}   

void BuitDevicesManager::changeScene(ControlCommand command){
    switch(command.commandType){
        case ROTATING_RIGHT:
            _sequencer.nextScene();
            break;
        case ROTATING_LEFT:
            _sequencer.previousScene();
            break;
    }
    presentScene();
}

void  BuitDevicesManager::nudgePage(ControlCommand command){
    if (_concreteSequencer.nudgePageInSelectedSequence(command)) {
        showSequence();
    }
}

void BuitDevicesManager::selectParameter(ControlCommand command){
    _sequencer.selectParameter(command.value);
}

void BuitDevicesManager::rotateParameter(ControlCommand command){
    switch(command.commandType){
        case ROTATING_RIGHT:
            _sequencer.increaseParameter();
            break;
        case ROTATING_LEFT:
            _sequencer.decreaseParameter();
            break;
    }
}

void BuitDevicesManager::presentSequenceSelect(){
    writeSceneToNeoTrellis(_sequencer.getSequencesState());
}

void BuitDevicesManager::presentTransport(){
    _settingsPresenter.presentTransport(getCurrentBPM(), getSyncMode());
}

void BuitDevicesManager::presentBuitCC(){
    printToScreen("CCs Matrix", "", "");
    _trellis.writeBuitCCStates(_matrixBuitCC.getBuitCCStates(), TRANSPORT_COLOR);
}

void BuitDevicesManager::selectScene(ControlCommand command){
    _sequencer.selectScene(command.value);
}

void BuitDevicesManager::selectSequence(ControlCommand command){
    _sequencer.selectSequence(command.value);
}

void BuitDevicesManager::editBuitCC(ControlCommand command){
    _matrixBuitCC.toggleBuitCC(command.value);
    _trellis.writeBuitCCStates(_matrixBuitCC.getBuitCCStates(), TRANSPORT_COLOR);
}

void BuitDevicesManager::sendBuitCC(ControlCommand command){
    _matrixBuitCC.updateAndSend(command);
}

int BuitDevicesManager::getSelectedSequenceMidichannel(){
    return _sequencer.getMidiChannel();
}

uint8_t BuitDevicesManager::getSelectedSequenceType(){
    return _sequencer.getSequenceType();
}

uint32_t BuitDevicesManager::getSelectedSequenceColor(){
    return _sequencer.getSequenceColor();
}

bool BuitDevicesManager::isSelectedSequenceRecording(){
    return _sequencer.isRecording();
}

bool BuitDevicesManager::acceptsInputFrom(uint8_t srcPort, uint8_t srcDevice){
    RTPEventNoteSequence* seq = _concreteSequencer.getActiveSequence();
    if (!seq) return true;
    return seq->acceptsInput(srcPort, srcDevice);
}

void BuitDevicesManager::playLiveNoteOn(uint8_t rootNote, uint8_t velocity, uint8_t chordType){
    _sequencer.playLiveNoteOn(rootNote, velocity, chordType);
}

void BuitDevicesManager::playLiveNoteOff(uint8_t rootNote, uint8_t chordType){
    _sequencer.playLiveNoteOff(rootNote, chordType);
}

void BuitDevicesManager::handleLiveThreeAxis(ControlCommand command){
    _concreteSequencer.handleLiveThreeAxis(command);
}

uint8_t BuitDevicesManager::getLiveVelocity(){
    return _sequencer.getLiveVelocity();
}

void BuitDevicesManager::saveSequencer(const String& fileName){
    if(_persistenceManager.saveSequencerToFile(_concreteSequencer, fileName))
        printToScreen("Saved", fileName, "");
    else
        printToScreen("Failed to save", fileName, "");
}

void BuitDevicesManager::loadSequencer(const String& fileName){
    if(_persistenceManager.loadSequencerFromFile(_concreteSequencer, fileName))
        printToScreen("Loaded", fileName, "");
    else
        printToScreen("Failed to load", fileName, "");
}

bool BuitDevicesManager::patternFileExists(const String& fileName){
    return _persistenceManager.fileExists(fileName);
}

void BuitDevicesManager::clearTrellis(){
    _trellis.clearAllButtons();
}

void BuitDevicesManager::setTrellisButtonColor(uint8_t index, uint32_t color){
    _trellis.setButtonColor(index, color);
}

void BuitDevicesManager::showTrellis(){
    _trellis.show();
}

uint32_t BuitDevicesManager::colorForPage(uint8_t page){
    return _trellis.getColorForPage(page);
}

uint32_t BuitDevicesManager::colorForSlot(uint8_t page, bool exists){
    return _trellis.getColorForSlot(page, exists);
}

void BuitDevicesManager::sceneAdd(){
    _sequencer.addDynamicScene();
    int count = _sequencer.getNumScenes();

    printToScreen("Scene Added", String(count) + " scenes", "");
}

void BuitDevicesManager::sceneRemove(){
    if (_sequencer.isPlaying()) {
        printToScreen("Stop first", "", "");
        return;
    }
    if (_sequencer.getNumScenes() <= 1) {
        printToScreen("Min 1 scene", "", "");
        return;
    }
    _sequencer.removeCurrentScene();
    int count = _sequencer.getNumScenes();
    printToScreen("Scene Removed", String(count) + " scenes", "");
}

void BuitDevicesManager::sceneToggleAll(){
    _sequencer.toggleAllSequences();
    presentScene();
}

int BuitDevicesManager::getSceneCount() const {
    return _sequencer.getNumScenes();
}

// Transport control methods
bool BuitDevicesManager::isInternalClock() const {
    return _clockGenerator && _clockGenerator->getMode() == SyncMode::Internal;
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
    if (!_clockGenerator) return SyncMode::External;
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
    _recordingManager.setQuantizeStrength(_quantizeStrength);
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