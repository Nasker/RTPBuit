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
    void update();  // per-loop: expire drum hit flashes

    void recorderNoteOn(uint8_t note, uint8_t velocity);
    void recorderNoteOff(uint8_t note);
    void recorderAdvancePulse();   // every raw 24-PPQN pulse
    void recorderDumpToSequence();
    void toggleSelectedSequenceRecording(bool fromPianoRoll = false);
    // After a take auto-finishes at loop end the UI returns to the view it was
    // armed from. Consumes the flag — 1 = piano roll, 0 = sequence edit, -1 = none.
    int8_t consumeRecordReturnView();
    bool isSelectedSequenceWaiting();
    SequenceDisplayState getSequenceDisplayState();

private:
    // Drum hit flash: millis() at which each pad's white press flash expires,
    // 0 = not flashing. Time-based so a missed release event can't latch a pad
    // white — the sweep restores it regardless of event delivery.
    uint32_t _drumFlashUntil[16] = {};
    static constexpr uint16_t DRUM_FLASH_MS = 140;
    void _sweepDrumFlashes();

    bool _recordFromPianoRoll = false;   // view the take was armed from
    bool _pendingViewReturn = false;     // auto-finished — UI should return to origin view

    RTPEventNoteSequence* _selectedSequence();
    uint8_t getSelectedSequenceType();
    uint32_t getSelectedSequenceColor();
    uint8_t getLiveVelocity();
    bool isSelectedSequenceRecording();
    void setTrellisButtonColor(uint8_t index, uint32_t color);
    void showTrellis();
    void printToScreen(String firstLine, String secondLine, String thirdLine);
};
