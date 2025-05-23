#include "BuitDevicesManager.hpp"

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
    _neoTrellis.writeTransportPage(TRANSPORT_COLOR);
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
    printToScreen("Scene",_sequencer.getSelectScene(),"");
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
    
    // Use the new four-line display method with recording status
    _oled.printToScreen(
        sequenceType,
        "Sequence "+ String(_sequencer.getSelectedSequence()+1),
        "Page "+ String(currentPage) + " of " + String(totalPages),
        "Ch " + String(midiChannel),
        _sequencer.isSelectedSequenceRecording() // Pass recording status
    );
    
    writeSequenceToNeoTrellis(_sequencer.getSelectedSequenceNoteStates(), _sequencer.getSelectedSequenceColor()); 
}

void BuitDevicesManager::presentTransport(){
    printToScreen("Transport", "", "");
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

void BuitDevicesManager::toggleSelectedSequenceRecording(){
    _sequencer.toggleSelectedSequenceRecording();
    // Update display to show the new recording status
    showSequence();
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
    