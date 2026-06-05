#pragma once

#include "RTPSequencer.h"

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
    uint8_t counter;
public:
    RTPSequencerManager(RTPSequencer& sequencer);
    void begin(RTPMainUnit* _mainUnit);
    void handleRealTimeSystem(uint8_t realtimebyte);
    uint8_t getCurrentCounter() { return counter; }
    int getNearestStepPosition();
private:
    void gridClockUp(uint8_t realtimebyte);
    void increaseCounter();
    void resetCounter();
};