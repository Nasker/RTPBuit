#include "RTPSequencerManager.hpp"
#include "RTPMainUnit.hpp"
#include "Config/MusicConfig.hpp"
#include "Midi/MidiRouter.hpp"
#include "Midi/MidiMessage.hpp"

RTPMainUnit* RTPSequencerManager::mainUnit;

RTPSequencerManager::RTPSequencerManager(RTPSequencer& seq):_sequencer(seq){
    resetCounter();
}

void RTPSequencerManager::begin(RTPMainUnit* _mainUnit){
    RTPSequencerManager::mainUnit = _mainUnit;
    //_sequencer.connectMainUnit(_mainUnit);
}


void RTPSequencerManager::update(){
    if (!_clockGenerator || _clockGenerator->getMode() != SyncMode::Internal) return;

    if (_clockGenerator->update()) {
        dispatchRealTime(MIDI_RT_CLOCK);
        _clockGenerator->clearPendingClock();
    }

    if (_clockGenerator->shouldSendStart()) {
        dispatchRealTime(MIDI_RT_START);
        _clockGenerator->clearPendingStart();
        dispatchRealTime(MIDI_RT_CLOCK);  // Immediate clock - eliminates first-step delay
    }

    if (_clockGenerator->shouldSendStop()) {
        dispatchRealTime(MIDI_RT_STOP);
        _clockGenerator->clearPendingStop();
    }
}

void RTPSequencerManager::dispatchRealTime(uint8_t realtimebyte){
    handleRealTimeSystem(realtimebyte);
    if (_clockGenerator && _clockGenerator->isSendingMidiRealtime() && _midiRouter) {
        MidiMessage msg { MidiMessage::RealTime, 0, realtimebyte, 0, MidiPort::INTERNAL };
        _midiRouter->route(msg);
    }
}

void RTPSequencerManager::handleRealTimeSystem(uint8_t realtimebyte){
	switch (realtimebyte) {
        case MIDI_RT_START:
        case MIDI_RT_CONTINUE:
            _sequencer.play();
            sendTransportCallback(TRANSPORT_START);
            break;
        case MIDI_RT_STOP:
            _sequencer.stop();
            resetCounter();
            sendTransportCallback(TRANSPORT_STOP);
            break;
        case MIDI_RT_CLOCK:
            gridClockUp(realtimebyte);
            break;
        default:
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
    if (counter % MusicConfig::Timing::CLOCK_GRID == 0){
        _sequencer.play();
        ControlCommand callbackCommand;
        callbackCommand.controlType = SEQUENCER;
        callbackCommand.commandType = GRID_TICK;
        callbackCommand.value = counter;
        mainUnit->actOnSequencerCallback(callbackCommand);
    }

    // Finer 32nd-note callback for live rolls (does NOT advance the sequencer step)
    if (counter % MusicConfig::Timing::FINE_GRID == 0){
        ControlCommand fineCommand;
        fineCommand.controlType = SEQUENCER;
        fineCommand.commandType = GRID_FINE_TICK;
        fineCommand.value = counter;
        mainUnit->actOnSequencerCallback(fineCommand);
    }

    increaseCounter();
} 

void RTPSequencerManager::increaseCounter(){
    counter++;
    if (counter == MusicConfig::Timing::TICKS_PER_BAR) 
        resetCounter();
}

void RTPSequencerManager::resetCounter(){
    counter = 0;
}

int RTPSequencerManager::getNearestStepPosition() {
    // Calculate the current position in the sequence
    int currentPos = _sequencer.getCurrentPosition();
    
    int positionInStep = counter % MusicConfig::Timing::CLOCK_GRID;
    
    if (positionInStep < MusicConfig::Timing::CLOCK_GRID / 2) {
        return currentPos;
    } else {
        int nextPos = currentPos + 1;
        if (nextPos >= (int)_sequencer.getSequenceLength()) {
            nextPos = 0;
        }
        return nextPos;
    }
}