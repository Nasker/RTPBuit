#include "MatrixBuitControlChanger.hpp"

MatrixBuitControlChanger::MatrixBuitControlChanger(){
    for(int i=0; i < N_BUITS_CC; i++)
        controlChangers.push_back(BuitControlChanger(i));
}

void MatrixBuitControlChanger::toggleBuitCC(int buitID){
    controlChangers[buitID].isEnabled() ? controlChangers[buitID].disable() : controlChangers[buitID].enable();
}

RTPSequencesState MatrixBuitControlChanger::getBuitCCStates(){
    RTPSequencesState sequencesState;
    for(int i=0; i < N_BUITS_CC; i++)
        sequencesState.sequenceState[i].state = controlChangers[i].isEnabled();
    return sequencesState;
}

void MatrixBuitControlChanger::updateAndSend(ControlCommand command){
    if(command.commandType != CHANGE_CENTER){
        for(int i=0; i < N_BUITS_CC; i++){
            if(controlChangers[i].isEnabled())
                controlChangers[i].updateAndSend(command);
        }
    }
}