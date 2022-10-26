#include "RTPSequencerManager.hpp"
#include "RTPMainUnit.hpp"

RTPMainUnit* RTPSequencerManager::mainUnit;

RTPSequencerManager::RTPSequencerManager(RTPSequencer& seq):_sequencer(seq){
    resetCounter();
}

void RTPSequencerManager::begin(RTPMainUnit* _mainUnit){
    RTPSequencerManager::mainUnit = _mainUnit;
}


void RTPSequencerManager::handleRealTimeSystem(byte realtimebyte){
	switch (realtimebyte) {
        case STOP:
            _sequencer.stopAndCleanSequencer();
            resetCounter();
            break;
        case CLOCK:
            gridClockUp(realtimebyte);
            break;
        default:
            // Serial.printf("RealTimeSystem: %d\n", realtimebyte);
            break;
	}
}

void RTPSequencerManager::gridClockUp(byte realtimebyte){
    if (counter % CLOCK_GRID == 0){
        _sequencer.playAndMoveSequencer();
        ControlCommand callbackCommand;
        callbackCommand.controlType = SEQUENCER;
        callbackCommand.commandType = GRID_TICK;
        callbackCommand.value = counter;
        mainUnit->actOnSequencerCallback(callbackCommand);
    }
        
    increaseCounter();
} 

void RTPSequencerManager::increaseCounter(){
    counter++;
    if (counter == TICKS_PER_BAR) 
        resetCounter();
}

void RTPSequencerManager::resetCounter(){
    counter = 0;
}