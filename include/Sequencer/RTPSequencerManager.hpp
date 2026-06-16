#pragma once

#include "RTPSequencer.h"
#include "RTPClockGenerator.hpp"
#include "Interfaces/ISequencer.hpp"
#include "Interfaces/IClockGenerator.hpp"
#include "constants.h"

#define CLOCK 248
#define START 250
#define CONTINUE 251
#define STOP 252
#define TICKS_PER_BAR 96
#define CLOCK_GRID 6
#define FINE_GRID 3   // 32nd-note resolution (24 PPQN / 3 = 8 per quarter)

class RTPMainUnit;
class RTPSequencerManager{
    static RTPMainUnit* mainUnit;
    ISequencer& _sequencer;
    IClockGenerator* _clockGenerator = nullptr;
    uint8_t counter;
public:
    RTPSequencerManager(RTPSequencer& sequencer);
    void begin(RTPMainUnit* _mainUnit);
    void setClockGenerator(IClockGenerator& clockGenerator) { _clockGenerator = &clockGenerator; }
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