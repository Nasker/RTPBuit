#pragma once

#include "Arduino.h"
#include "Interfaces/IDisplay.hpp"
#include "Interfaces/IInputDevice.hpp"
#include "Interfaces/ISequencer.hpp"
#include "RTPSequencer.h"
#include "Managers/RecordingManager.hpp"
#include "Managers/LivePlayManager.hpp"
#include "Structs.h"

class LivePlayOrchestrator {
    IDisplay& _display;
    IButtonMatrix& _trellis;
    ISequencer& _sequencer;
    RTPSequencer& _concreteSequencer;
    RecordingManager& _recordingManager;
    LivePlayManager& _livePlayManager;

public:
    LivePlayOrchestrator(IDisplay& display, IButtonMatrix& trellis,
                         ISequencer& sequencer, RTPSequencer& concreteSequencer,
                         RecordingManager& recordingManager, LivePlayManager& livePlayManager);

    void handleLiveTrellisPressed(uint8_t pad);
    void handleLiveTrellisReleased(uint8_t pad);
    void handleLiveSequencerTick();
    void handleLiveFineTick();
    void handleLiveDrumRollThreeAxis(ControlCommand command);
    void syncLiveTrellis();
    void paintLiveTrellis();

    void recorderNoteOn(uint8_t note, uint8_t velocity);
    void recorderNoteOff(uint8_t note);
    void recorderAdvanceTick();
    void recorderDumpToSequence();
    void toggleSelectedSequenceRecording();
    bool isSelectedSequenceWaiting();
    SequenceDisplayState getSequenceDisplayState();

private:
    uint8_t getSelectedSequenceType();
    uint32_t getSelectedSequenceColor();
    uint8_t getLiveVelocity();
    bool isSelectedSequenceRecording();
    void setTrellisButtonColor(uint8_t index, uint32_t color);
    void showTrellis();
    void printToScreen(String firstLine, String secondLine, String thirdLine);
};
