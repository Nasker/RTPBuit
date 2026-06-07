#include "RTPSequencerManager.hpp"
#include "RTPMainUnit.hpp"

RTPMainUnit* RTPSequencerManager::mainUnit;

RTPSequencerManager::RTPSequencerManager(RTPSequencer& seq):_sequencer(seq){
    resetCounter();
}

void RTPSequencerManager::begin(RTPMainUnit* _mainUnit){
    RTPSequencerManager::mainUnit = _mainUnit;
    //_sequencer.connectMainUnit(_mainUnit);
}


void RTPSequencerManager::handleRealTimeSystem(uint8_t realtimebyte){
	switch (realtimebyte) {
        case START:
        case CONTINUE:
            _sequencer.playAndMoveSequencer(); // Initialize playback immediately
            // Notify UI that transport has started
            sendTransportCallback(TRANSPORT_START);
            break;
        case STOP:
            _sequencer.stopAndCleanSequencer();
            resetCounter();
            // Notify UI that transport has stopped
            sendTransportCallback(TRANSPORT_STOP);
            break;
        case CLOCK:
            gridClockUp(realtimebyte);
            break;
        default:
            // Serial.printf("RealTimeSystem: %d\n", realtimebyte);
            break;
	}
}

void RTPSequencerManager::sendTransportCallback(uint8_t transportCommand){
    ControlCommand callbackCommand;
    callbackCommand.controlType = SEQUENCER;
    callbackCommand.commandType = transportCommand;
    callbackCommand.value = 0;
    mainUnit->actOnSequencerCallback(callbackCommand);
}

void RTPSequencerManager::gridClockUp(uint8_t realtimebyte){
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

int RTPSequencerManager::getNearestStepPosition() {
    // Calculate the current position in the sequence
    int currentPos = _sequencer.getSelectedSequencePosition();
    
    // Calculate the position within the current step (0 to CLOCK_GRID-1)
    int positionInStep = counter % CLOCK_GRID;
    
    // If we're in the first half of the step, quantize to the current step
    // If we're in the second half, quantize to the next step
    if (positionInStep < CLOCK_GRID / 2) {
        return currentPos;
    } else {
        // Calculate the next position, wrapping around if needed
        int nextPos = currentPos + 1;
        if (nextPos >= _sequencer.getSelectedSequenceSize()) {
            nextPos = 0;
        }
        return nextPos;
    }
}