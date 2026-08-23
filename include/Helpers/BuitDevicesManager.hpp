#pragma once

#include "Interfaces/IDisplay.hpp"
#include "Interfaces/IInputDevice.hpp"
#include "RTPSequencer.h"
#include "RTPSequencerManager.hpp"
#include "Interfaces/ISequencer.hpp"
#include "Interfaces/IClockGenerator.hpp"
#include "MatrixBuitControlChanger.hpp"
#include "Structs.h"
#include "RTPTypeColors.h"
#include "RTPSDManager.hpp"
#include "BuitPersistenceManager.hpp"
#include "Managers/RecordingManager.hpp"
#include "Managers/LivePlayManager.hpp"
#include "SequenceSettingsPresenter.hpp"
#include "LivePlayOrchestrator.hpp"

class UsbHostManager;

/**
 * @brief Device facade for the state machine - interface-based
 * 
 * Hardware access goes through interfaces, injected via constructor:
 * - Display → `IDisplay` (production: RTPOledAdapter, shared with DisplayManager)
 * - Button matrix → `IButtonMatrix` (production: RTPNeoTrellisAdapter, shared with InputManager)
 * - Sequencer → `ISequencer` / `RTPSequencer`
 * - Clock → `IClockGenerator`
 * - Recording → `RecordingManager` (composition)
 * - Live-play → `LivePlayManager` (composition)
 * - Persistence → `BuitPersistenceManager`
 * 
 * @see /include/Managers/RecordingManager.hpp
 * @see /include/Managers/LivePlayManager.hpp
 * @see /include/Hardware/Adapters/ for adapter pattern bridging legacy hardware
 */
class BuitDevicesManager {
    IDisplay& _display;
    IButtonMatrix& _trellis;
    ISequencer& _sequencer;
    RTPSequencer& _concreteSequencer;
    BuitPersistenceManager _persistenceManager;
    MatrixBuitControlChanger _matrixBuitCC;
    IClockGenerator* _clockGenerator = nullptr;
    UsbHostManager* _usbHostManager = nullptr;

    // Decomposed managers (composition pattern)
    RecordingManager _recordingManager;
    LivePlayManager _livePlayManager;
    SequenceSettingsPresenter _settingsPresenter;
    LivePlayOrchestrator _livePlayOrchestrator;

public:
    BuitDevicesManager(IDisplay& display, IButtonMatrix& trellis, RTPSequencer& sequencer);
    void initSetup();
    void introAnimations();
    void printToScreen(String firstLine, String secondLine, String thirdLine);

    void selectScene(ControlCommand command);
    void selectSequence(ControlCommand command);

    void presentScene()              { _settingsPresenter.presentScene(); }
    void presentSequenceSelect();
    void presentSequence()           { _settingsPresenter.showSequence(); }
    void paintLiveTrellis()          { _livePlayOrchestrator.paintLiveTrellis(); }
    void showSequence()              { _settingsPresenter.showSequence(); }
    void presentTransport();
    void presentBuitCC();
    void presentSequenceSettings()   { _settingsPresenter.presentSequenceSettings(); }

    void editScene(ControlCommand command);
    void editSequence(ControlCommand command);
    void useTransport(ControlCommand command);
    void editCurrentNote(ControlCommand command);

    void displayCursorInSequence(ControlCommand command);

    void changeScene(ControlCommand command);
    void nudgePage(ControlCommand command);

    void selectParameter(ControlCommand command);
    void rotateParameter(ControlCommand command);

    void editBuitCC(ControlCommand command);
    void sendBuitCC(ControlCommand command);

    int getSelectedSequenceMidichannel();
    uint8_t getSelectedSequenceType();
    uint32_t getSelectedSequenceColor();
    bool isSelectedSequenceRecording();
    bool acceptsInputFrom(uint8_t srcPort, uint8_t srcDevice);
    void playLiveNoteOn(uint8_t rootNote, uint8_t velocity, uint8_t chordType);
    void playLiveNoteOff(uint8_t rootNote, uint8_t chordType);
    void handleLiveThreeAxis(ControlCommand command);
    uint8_t getLiveVelocity();

    // Live-play orchestration
    void handleLiveTrellisPressed(uint8_t pad)  { _livePlayOrchestrator.handleLiveTrellisPressed(pad); }
    void handleLiveTrellisReleased(uint8_t pad) { _livePlayOrchestrator.handleLiveTrellisReleased(pad); }
    void handleLiveSequencerTick()              { _livePlayOrchestrator.handleLiveSequencerTick(); }
    void handleLiveFineTick()                   { _livePlayOrchestrator.handleLiveFineTick(); }
    void handleLiveDrumRollThreeAxis(ControlCommand command) { _livePlayOrchestrator.handleLiveDrumRollThreeAxis(command); }
    void syncLiveTrellis()                      { _livePlayOrchestrator.syncLiveTrellis(); }
    bool isSelectedSequenceWaiting()            { return _livePlayOrchestrator.isSelectedSequenceWaiting(); }
    SequenceDisplayState getSequenceDisplayState() { return _livePlayOrchestrator.getSequenceDisplayState(); }
    void toggleSelectedSequenceRecording()       { _livePlayOrchestrator.toggleSelectedSequenceRecording(); }

    void recorderNoteOn(uint8_t note, uint8_t velocity) { _livePlayOrchestrator.recorderNoteOn(note, velocity); }
    void recorderNoteOff(uint8_t note)                  { _livePlayOrchestrator.recorderNoteOff(note); }
    void recorderAdvanceTick()                           { _livePlayOrchestrator.recorderAdvanceTick(); }
    void recorderDumpToSequence()                        { _livePlayOrchestrator.recorderDumpToSequence(); }

    void saveSequencer(const String& fileName);
    void loadSequencer(const String& fileName);
    bool patternFileExists(const String& fileName);

    // Scene management
    void sceneAdd();
    void sceneRemove();
    void sceneToggleAll();
    int  getSceneCount() const;
    void presentSceneSettings(int8_t focusedPad = -1) { _settingsPresenter.presentSceneSettings(focusedPad); }

    // Direct trellis access for pattern bank UI
    void clearTrellis();
    void setTrellisButtonColor(uint8_t index, uint32_t color);
    void showTrellis();
    uint32_t colorForPage(uint8_t page);
    uint32_t colorForSlot(uint8_t page, bool exists);

    // Sequencer and music manager access
    ISequencer& getSequencer() { return _sequencer; }
    RTPSequencer& getConcreteSequencer() { return _concreteSequencer; }
    MusicManager& getMusicManager() { return _concreteSequencer.getMusicManager(); }

    // Clock generator access (set by RTPMainUnit)
    void setClockGenerator(IClockGenerator& clockGenerator) { _clockGenerator = &clockGenerator; }
    void setUsbHostManager(UsbHostManager* mgr) { _usbHostManager = mgr; _settingsPresenter.setUsbHostManager(mgr); }
    
    // Transport control (delegate to clock generator if available)
    bool isInternalClock() const;
    void transportPlay();
    void transportStop();
    void transportTapTempo();
    void transportToggleMode();
    void transportIncrementBPM(float delta);
    void transportSetBPM(float bpm);
    float getCurrentBPM() const;
    SyncMode getSyncMode() const;
    
    // Parameter adjustments for transport rotary control
    void incrementSwing(int delta);
    void setQuantizeStrength(int strength);
    void incrementQuantizeStrength(int delta);
    void incrementMasterVolume(int delta);
    
    // Get current parameter values for display
    int getSwing() const { return _swingAmount; }
    int getQuantizeStrength() const { return _quantizeStrength; }
    int getMasterVolume() const { return _masterVolume; }
    
private:
    int _swingAmount = 0;               // 0-100%
    int _quantizeStrength = 50;         // 0-100%
    int _masterVolume = 100;            // 0-100%
    void writeSequenceToNeoTrellis(RTPSequenceNoteStates sequenceStates, int color);
    void writeSceneToNeoTrellis(RTPSequencesState sequencesState);
};