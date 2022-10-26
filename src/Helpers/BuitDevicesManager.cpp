#include "BuitDevicesManager.hpp"

BuitDevicesManager::BuitDevicesManager(RTPNeoTrellis& nT, RTPSequencer& seq):
_neoTrellis(nT),
_sequencer(seq){}

void BuitDevicesManager::initSetup(){
    _oled.init();
    initBuitSD();
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
    printToScreen("Edit Sequence", "", "");
    _sequencer.toggleNoteInSceneInSelectedSequence(command.value);
    _neoTrellis.writeSequenceStates(_sequencer.getSelectedSequenceNoteStates(), _sequencer.getSelectedSequenceColor());
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
        Serial.printf("Display Cursor in Sequence Position: %d\n", _sequencer.getSelectedSequencePosition());
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
    writeSequenceToNeoTrellis(_sequencer.getSelectedSequenceNoteStates(), _sequencer.getSelectedSequenceColor());
}

void BuitDevicesManager::presentScene(){
    printToScreen("Edit Scene","","");
    writeSceneToNeoTrellis(_sequencer.getSequencesState());
}

void BuitDevicesManager::presentSequence(){
    printToScreen("Edit Sequence","","");
    writeSequenceToNeoTrellis(_sequencer.getSelectedSequenceNoteStates(), _sequencer.getSelectedSequenceColor()); 
}

void BuitDevicesManager::presentTransport(){
    printToScreen("Transport", "", "");
    writeTransportPage();
}

void BuitDevicesManager::presentBuitCC(){
    printToScreen("Buit CCs", "", "");
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

void BuitDevicesManager::dumpSequencesToJson(){
    _sequencer.dumpSequencesToJson();
}