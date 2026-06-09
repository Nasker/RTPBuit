#pragma once

#include "RTPSequencer.h"
#include "RTPClockGenerator.hpp"
#include "constants.h"

#define CLOCK 248
#define START 250
#define CONTINUE 251
#define STOP 252
#define TICKS_PER_BAR 96
#define CLOCK_GRID 6

class RTPMainUnit;
class RTPSequencerManager{
    static RTPMainUnit* mainUnit;
    RTPSequencer& _sequencer;
    RTPClockGenerator* _clockGenerator = nullptr;
    uint8_t counter;
public:
    RTPSequencerManager(RTPSequencer& sequencer);
    void begin(RTPMainUnit* _mainUnit);
    void setClockGenerator(RTPClockGenerator& clockGenerator) { _clockGenerator = &clockGenerator; }
    void update();  // Poll internal clock and dispatch events
    void handleRealTimeSystem(uint8_t realtimebyte);
    uint8_t getCurrentCounter() { return counter; }
    int getNearestStepPosition();
private:
    void dispatchRealTime(uint8_t realtimebyte);  // Route to sequencer + MIDI output
    void gridClockUp(uint8_t realtimebyte);
    void sendTransportCallback(uint8_t transportCommand);
    void increaseCounter();
    void resetCounter();
};