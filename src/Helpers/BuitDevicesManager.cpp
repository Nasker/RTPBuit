#include "BuitDevicesManager.hpp"
#include "Sequencer/RTPEventNoteSequence.h"

BuitDevicesManager::BuitDevicesManager(RTPNeoTrellis& nT, RTPSequencer& seq):
_neoTrellis(nT),
_sequencer(seq),
_concreteSequencer(seq){}

void BuitDevicesManager::initSetup(){
    _oled.init();
    initBuitSD();
    introAnimations();
    
    // Initialize new managers
    _recordingManager.initialize();
    _livePlayManager.initialize();
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
    if (getSyncMode() == SyncMode::Internal) {
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
    _sequencer.toggleNote(command.value);
}

void BuitDevicesManager::editCurrentNote(ControlCommand command){
    _concreteSequencer.editNoteInCurrentPosition(command);
}



void BuitDevicesManager::displayCursorInSequence(ControlCommand command){
    int cursorPos = _sequencer.getCurrentPosition() - _sequencer.getPageOffset();
    if (cursorPos >= 0 && cursorPos < SEQ_BLOCK_SIZE){
        _neoTrellis.writeSequenceStates(_sequencer.getNoteStates(), _sequencer.getSequenceColor(), false);
        _neoTrellis.moveCursor(cursorPos);
    }
    else {
        _neoTrellis.writeSequenceStates(_sequencer.getNoteStates(), _sequencer.getSequenceColor());
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
    _concreteSequencer.nudgePageInSelectedSequence(command);
    showSequence();
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

void BuitDevicesManager::presentScene(){
    // Determine scene display state based on sequencer playing status
    SequenceDisplayState state = _sequencer.isPlaying() ? SequenceDisplayState::Playing : SequenceDisplayState::Stopped;
    
    _oled.printToScreen(
        "Scene",
        "Scene " + String(_sequencer.getCurrentScene() + 1),
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

void BuitDevicesManager::paintLiveTrellis() {
    _livePlayManager.releaseAllChords();
    uint8_t seqType = getSelectedSequenceType();
    uint32_t seqColor = getSelectedSequenceColor();
    clearTrellis();

    static const bool isBlackKey[12] = {false,true,false,true,false,false,true,false,true,false,true,false};

    if (seqType == DRUM_PART) {
        for (uint8_t i = 0; i < 16; i++)
            setTrellisButtonColor(i, seqColor);
    } else {
        for (uint8_t i = 0; i < 12; i++) {
            uint32_t dimmed = (((seqColor >> 16 & 0xFF) * 35 / 255) << 16)
                            | (((seqColor >>  8 & 0xFF) * 35 / 255) <<  8)
                            |  ((seqColor       & 0xFF) * 35 / 255);
            setTrellisButtonColor(i, isBlackKey[i] ? dimmed : seqColor);
        }
        for (uint8_t i = 0; i < N_CHORDION_KEYS; i++)
            setTrellisButtonColor(12 + i, 0x101010);
    }
    showTrellis();
}

void BuitDevicesManager::showSequence(){
    // Get the sequence type name
    String sequenceType = _sequencer.getSequenceTypeName();
    
    int midiChannel = _sequencer.getMidiChannel();
    
    int currentPage = _sequencer.getCurrentPage() + 1;
    int totalPages = 4; // Assuming 4 pages total
    
    // Determine display state
    SequenceDisplayState state = getSequenceDisplayState();
    
    // Update blink counter for waiting state (toggle every 8 calls ~ 133ms at 60fps)
    _displayBlinkCounter++;
    bool blinkState = (_displayBlinkCounter / 8) % 2 == 0;
    
    // Use the state-based display method
    _oled.printToScreen(
        sequenceType,
        "Sequence "+ String(_sequencer.getCurrentSequence()+1),
        "Page "+ String(currentPage) + " of " + String(totalPages),
        "Ch " + String(midiChannel),
        state,
        blinkState
    );
    
    writeSequenceToNeoTrellis(_sequencer.getNoteStates(), _sequencer.getSequenceColor()); 
}

void BuitDevicesManager::presentTransport(){
    String modeStr = (getSyncMode() == SyncMode::Internal) ? "INT" : "EXT";
    String playState = _sequencer.isPlaying() ? "Playing" : "Stopped";
    String bpmStr = (getSyncMode() == SyncMode::Internal) ? String(getCurrentBPM(), 0) + " BPM" : "";
    
    SequenceDisplayState state = _sequencer.isPlaying() ? SequenceDisplayState::Playing : SequenceDisplayState::Stopped;
    
    _oled.printToScreen(
        "Transport",
        playState + "  " + modeStr,
        bpmStr,
        "Scene " + String(_sequencer.getCurrentScene() + 1),
        state,
        false
    );
    
    writeTransportPage();
}

void BuitDevicesManager::presentSequenceSettings(){
    SequenceSettings s = _concreteSequencer.getSelectedSequenceSettings();
    String paramName  = _sequencer.getParameterName();
    int    paramValue = _sequencer.getParameterValue();

    String valueStr;
    if (paramName == "Type") {
        valueStr = _sequencer.getSequenceTypeName();
    } else if (paramName == "Midi CH") {
        valueStr = "CH " + String(paramValue);
    } else if (paramName == "Color") {
        valueStr = "Col " + String(paramValue);
    } else if (paramName == "Lenght") {
        valueStr = String(paramValue) + " pages";
    } else {
        valueStr = String(paramValue);
    }

    printToScreen("Seq Settings", paramName, valueStr);
    _neoTrellis.writeSequenceSettingsPage(s);
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

void BuitDevicesManager::handleLiveTrellisPressed(uint8_t pad) {
    uint8_t seqType = getSelectedSequenceType();
    uint8_t liveVel = getLiveVelocity();

    if (seqType == DRUM_PART) {
        uint8_t note = BASE_NOTE + pad;
        playLiveNoteOn(note, liveVel, 0);
        if (isSelectedSequenceRecording())
            recorderNoteOn(note, liveVel);
        setTrellisButtonColor(pad, 0xFFFFFF);
        return;
    }

    // Melodic (Poly / Mono / Bass)
    if (pad < 12) {
        uint8_t rootNote = BASE_NOTE + pad;
        uint8_t chordType;

        if (seqType == POLY_SYNTH) {
            // Poly: polyphonic, full chord, momentary (released on pad release)
            ChordAction action = _livePlayManager.beginChord(rootNote, pad);
            for (uint8_t i = 0; i < action.stopCount; i++)
                playLiveNoteOff(action.notesToStop[i], action.chordType);
            for (uint8_t i = 0; i < action.playCount; i++)
                playLiveNoteOn(action.notesToPlay[i], liveVel, action.chordType);
            chordType = action.chordType;
        } else {
            // Bass/Mono: monophonic + LATCHED. The root press snapshots root + the
            // current top-row type and holds it. Keep exactly one active chord so the
            // LED shows the latched root; the sequence glides (legato) to the new note.
            _livePlayManager.releaseAllChords();
            ChordAction action = _livePlayManager.beginChord(rootNote, pad);
            playLiveNoteOn(rootNote, liveVel, action.chordType);
            chordType = action.chordType;
        }

        if (isSelectedSequenceRecording()) {
            // Recorder tracks the root note
            recorderNoteOn(rootNote, liveVel);
        }

        setTrellisButtonColor(pad, 0xFFFFFF);
        String chordStr = String(NOTE_NAMES[pad % 12]) + " " + String(CHORD_TYPE_NAMES[chordType & 0x0F]);
        printToScreen("Piano Roll", _sequencer.getSequenceTypeName(), chordStr);
    } else {
        // Modifier pads (12-15)
        uint8_t modIdx = pad - 12;
        _livePlayManager.enableChordionKey(modIdx);
        setTrellisButtonColor(pad, 0xFFFFFF);
        String preview = String(CHORD_TYPE_NAMES[_livePlayManager.getChordType() & 0x0F]);
        printToScreen("Piano Roll", _sequencer.getSequenceTypeName(), "[ " + preview + " ]");
    }
}

void BuitDevicesManager::handleLiveTrellisReleased(uint8_t pad) {
    uint8_t seqType = getSelectedSequenceType();

    if (seqType == DRUM_PART) {
        uint8_t note = BASE_NOTE + pad;
        playLiveNoteOff(note, 0);
        if (isSelectedSequenceRecording())
            recorderNoteOff(note);
        setTrellisButtonColor(pad, getSelectedSequenceColor());
        return;
    }

    // Melodic
    if (pad < 12) {
        uint8_t rootNote = BASE_NOTE + pad;

        if (seqType == POLY_SYNTH) {
            // Poly is momentary: releasing the pad stops the chord
            ChordAction action = _livePlayManager.endChord(rootNote, pad);
            for (uint8_t i = 0; i < action.stopCount; i++)
                playLiveNoteOff(action.notesToStop[i], action.chordType);
            syncLiveTrellis();
        }
        // Bass/Mono: LATCHED -> do nothing on release. The chord keeps sounding
        // (presence-gated by the axis) and stays lit until another root is pressed.

        if (isSelectedSequenceRecording()) {
            recorderNoteOff(rootNote);
        }
    }

    // Modifier pad release (12-15): disable key (momentary), restore LED, update OLED preview
    if (pad >= 12 && pad <= 15) {
        _livePlayManager.disableChordionKey(pad - 12);
        setTrellisButtonColor(pad, 0x101010);
        String preview = String(CHORD_TYPE_NAMES[_livePlayManager.getChordType() & 0x0F]);
        printToScreen("Piano Roll", _sequencer.getSequenceTypeName(), "[ " + preview + " ]");
    }
}

void BuitDevicesManager::handleLiveSequencerTick() {
    // 16th-note grid tick: only used for display sync now.
    // All rolls run on the finer 32nd-note tick (handleLiveFineTick).
    _livePlayManager.advanceTick();
    _sequencer.handleLiveTick();
    if ((_livePlayManager.getTickCount() % 4) == 0)
        syncLiveTrellis();
}

void BuitDevicesManager::handleLiveFineTick() {
    // 32nd-note resolution tick: drives all live rolls.
    _livePlayManager.advanceFineTick();
    uint8_t seqType = getSelectedSequenceType();

    // Drum roll: re-trigger at division (1=32nd, 2=16th, 4=8th, ...)
    if (seqType == DRUM_PART && _livePlayManager.shouldTriggerDrumRoll()) {
        playLiveNoteOn(_livePlayManager.getDrumRollNote(), getLiveVelocity(), 0);
    }

    // Melodic rolls (Bass/Mono) handle their own division internally
    _sequencer.handleLiveFineTick();
}

void BuitDevicesManager::syncLiveTrellis() {
    uint8_t seqType = getSelectedSequenceType();
    uint32_t seqColor = getSelectedSequenceColor();
    uint16_t heldPads = _livePlayManager.getHeldPadsMask();
    static const bool isBlackKey[12] = {false,true,false,true,false,false,true,false,true,false,true,false};
    uint32_t dimmed = (((seqColor >> 16 & 0xFF) * 35 / 255) << 16)
                    | (((seqColor >>  8 & 0xFF) * 35 / 255) <<  8)
                    |  ((seqColor       & 0xFF) * 35 / 255);
    for (uint8_t i = 0; i < 16; i++) {
        bool pressed = (heldPads >> i) & 1;
        if (pressed) {
            setTrellisButtonColor(i, 0xFFFFFF);
        } else if (seqType == DRUM_PART) {
            setTrellisButtonColor(i, seqColor);
        } else if (i < 12) {
            setTrellisButtonColor(i, isBlackKey[i] ? dimmed : seqColor);
        } else {
            setTrellisButtonColor(i, 0x101010);
        }
    }
}

void BuitDevicesManager::handleLiveDrumRollThreeAxis(ControlCommand command) {
    int val = command.value;
    switch (command.commandType) {
        case CHANGE_LEFT:
            // Note selection (like Mono/Bass left axis selects note)
            if (val <= 2) {
                _livePlayManager.setDrumRollActive(false);
            } else {
                _livePlayManager.setDrumRollActive(true);
                uint8_t zone = (uint8_t)constrain((val * 16) / 127, 0, 15);
                _livePlayManager.setDrumRollNote(BASE_NOTE + zone);
            }
            break;
        case CHANGE_CENTER:
            // Roll speed - consistent with Mono/Bass: higher value = slower roll
            if (val >= 125) {
                _livePlayManager.setDrumRollActive(false);
            } else {
                _livePlayManager.setDrumRollActive(true);
                // Inverted: 0-10 = fastest 32nd (1), 120 = slowest half note (16)
                // division values: 1=32nd, 2=16th, 4=8th, 8=quarter, 16=half
                _livePlayManager.setRollDivision(::map(constrain(val, 0, 120), 0, 120, 1, 16));
            }
            break;
        case CHANGE_RIGHT:
            // Velocity - handled by main threeAxis handler
            handleLiveThreeAxis(command);
            break;
    }
}

bool BuitDevicesManager::isSelectedSequenceWaiting(){
    return _recordingManager.isWaiting();
}

SequenceDisplayState BuitDevicesManager::getSequenceDisplayState(){
    if (_recordingManager.isRecording()) {
        return SequenceDisplayState::Recording;
    } else if (_recordingManager.isWaiting()) {
        return SequenceDisplayState::Waiting;
    } else if (_sequencer.isRecording()) {
        // Sequencer thinks it's recording but recorder is not yet active (shouldn't happen, but handle it)
        return SequenceDisplayState::Waiting;
    } else {
        return SequenceDisplayState::Playing;
    }
}

void BuitDevicesManager::toggleSelectedSequenceRecording(){
    _sequencer.toggleRecording();
    if (_sequencer.isRecording()) {
        uint16_t seqSize = _sequencer.getSequenceLength();
        uint8_t midiChannel = _sequencer.getMidiChannel();
        uint16_t currentPos = _sequencer.getCurrentPosition();
        _recordingManager.startRecording(seqSize, midiChannel, currentPos);
    } else {
        _recordingManager.stopRecording();
        recorderDumpToSequence();
    }
    showSequence();
}

void BuitDevicesManager::recorderNoteOn(uint8_t note, uint8_t velocity) {
    if (_recordingManager.isRecording())
        _recordingManager.recordNoteOn(note, velocity);
}

void BuitDevicesManager::recorderNoteOff(uint8_t note) {
    if (_recordingManager.isRecording())
        _recordingManager.recordNoteOff(note);
}

void BuitDevicesManager::recorderAdvanceTick() {
    // Advance tick if recording OR waiting to start
    if (!_recordingManager.isRecording() && !_recordingManager.isWaiting()) return;
    
    _recordingManager.advanceTick();
    
    // Check for end of one-shot recording
    if (_recordingManager.isRecording() && _recordingManager.isEndOfSequence()) {
        _recordingManager.stopRecording();
        
        // Only dump if we actually recorded something
        auto notes = _recordingManager.dumpRecordedSequence();
        if (!notes.empty()) {
            // Apply to sequence
            RTPScene* scene = _concreteSequencer.getScene(_concreteSequencer.getSelectScene());
            if (scene) {
                RTPEventNoteSequence* seq = scene->getSequence(_concreteSequencer.getSelectedSequence());
                if (seq) {
                    uint16_t seqSize = _recordingManager.getSequenceLength();
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
        _sequencer.toggleRecording();
        showSequence();
    }
}

void BuitDevicesManager::recorderDumpToSequence() {
    auto notes = _recordingManager.dumpRecordedSequence();
    if (notes.empty()) return;
    RTPScene* scene = _concreteSequencer.getScene(_concreteSequencer.getSelectScene());
    if (!scene) return;
    RTPEventNoteSequence* seq = scene->getSequence(_concreteSequencer.getSelectedSequence());
    if (!seq) return;
    uint16_t seqSize = _recordingManager.getSequenceLength();
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
    _neoTrellis.clearAllButtons();
}

void BuitDevicesManager::setTrellisButtonColor(uint8_t index, uint32_t color){
    _neoTrellis.setButtonColor(index, color);
}

void BuitDevicesManager::showTrellis(){
    _neoTrellis.show();
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

void BuitDevicesManager::presentSceneSettings(int8_t focusedPad){
    bool playing  = _sequencer.isPlaying();
    int  nScenes  = _sequencer.getNumScenes();
    int  curScene = _sequencer.getCurrentScene() + 1;

    _neoTrellis.clearAllButtons();

    // Pad 0 — Load (cyan)
    _neoTrellis.setButtonColor(0, _neoTrellis.colorDim(_neoTrellis.colorBlue(), 200));

    // Pad 1 — Save (magenta: full red + blue)
    _neoTrellis.setButtonColor(1, _neoTrellis.colorDim(_neoTrellis.colorRed(), 180));

    // Pad 2 — Add scene (green, dim if playing)
    _neoTrellis.setButtonColor(2, playing
        ? _neoTrellis.colorDim(_neoTrellis.colorGreen(), 40)
        : _neoTrellis.colorGreen());

    // Pad 3 — Remove scene (red, dim if playing or only 1 scene)
    bool canRemove = !playing && nScenes > 1;
    _neoTrellis.setButtonColor(3, canRemove
        ? _neoTrellis.colorRed()
        : _neoTrellis.colorDim(_neoTrellis.colorRed(), 40));

    // Pad 4 — Toggle all sequences in scene (white)
    _neoTrellis.setButtonColor(4, _neoTrellis.colorWhite());

    // Highlight focused pad with full brightness white overlay
    if (focusedPad >= 0 && focusedPad <= 4)
        _neoTrellis.setButtonColor(focusedPad, _neoTrellis.colorWhite());

    _neoTrellis.show();

    printToScreen(
        "Scene Settings",
        "Scene " + String(curScene) + "/" + String(nScenes),
        playing ? "Playing" : "Stopped"
    );
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