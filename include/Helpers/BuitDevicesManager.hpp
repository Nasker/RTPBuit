#pragma once

#include "RTPOled.hpp"
#include "RTPNeoTrellis.hpp"
#include "RTPSequencer.h"
#include "RTPSequencerManager.hpp"
#include "Interfaces/ISequencer.hpp"
#include "Interfaces/IClockGenerator.hpp"
#include "MatrixBuitControlChanger.hpp"
#include "Structs.h"
#include "RTPTypeColors.h"
#include "RTPSDManager.hpp"
#include "BuitPersistenceManager.hpp"
#include "Helpers/NotesRecorder.hpp"
#include "Helpers/RTPClockGenerator.hpp"
#include "BuitFunctions/ChordionKeys.hpp"

/**
 * @brief Legacy device manager (God Object - 759 lines)
 * 
 * @deprecated This class violates Single Responsibility Principle by handling:
 * - Display operations (should use DisplayManager)
 * - Input handling (should use InputManager)
 * - Transport controls (should use TransportManager)
 * - Recording (NotesRecorder)
 * - Persistence (BuitPersistenceManager)
 * - Live-play orchestration (ChordionKeys, drum rolls)
 * 
 * @note Currently in production and working. Refactoring deferred due to:
 * - Deep coupling with state machine (10 state classes depend on this)
 * - Complex live-play logic (chordion, drum rolls, legato, presence gating)
 * - High risk of regression if migrated in one step
 * 
 * @see /include/Managers/DeviceManager.hpp for decomposed architecture
 * @see /include/Hardware/Adapters/ for adapter pattern bridging legacy hardware
 * 
 * Migration path: Gradually move state classes to use DeviceManager, then deprecate this class.
 */
class BuitDevicesManager {
    RTPOled _oled;
    RTPNeoTrellis& _neoTrellis;
    ISequencer& _sequencer;
    RTPSequencer& _concreteSequencer;
    BuitPersistenceManager _persistenceManager;
    MatrixBuitControlChanger _matrixBuitCC;
    NotesRecorder _notesRecorder;
    IClockGenerator* _clockGenerator = nullptr;
    ChordionKeys _chordionKeys;

    // Live-play state (moved from SequencePianoRollState)
    bool     _drumRollActive = false;
    uint8_t  _drumRollNote   = 36;
    uint8_t  _rollDivision   = 1;
    uint32_t _tickCount      = 0;
    uint32_t _fineTickCount  = 0;

public:
    BuitDevicesManager(RTPNeoTrellis& neoTrellis, RTPSequencer& sequencer);
    void initSetup();
    void introAnimations();
    void printToScreen(String firstLine, String secondLine, String thirdLine);
    void printToScreen(ControlCommand command);

    void selectScene(ControlCommand command);
    void selectSequence(ControlCommand command);

    void presentScene();
    void presentSequenceSelect();
    void presentSequence();
    void paintLiveTrellis();
    void showSequence();
    void presentTransport();
    void presentBuitCC();
    void presentSequenceSettings();

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
    void playLiveNoteOn(uint8_t rootNote, uint8_t velocity, uint8_t chordType);
    void playLiveNoteOff(uint8_t rootNote, uint8_t chordType);
    void handleLiveThreeAxis(ControlCommand command);
    uint8_t getLiveVelocity();

    // Live-play orchestration (moved from SequencePianoRollState)
    void handleLiveTrellisPressed(uint8_t pad);
    void handleLiveTrellisReleased(uint8_t pad);
    void handleLiveSequencerTick();
    void handleLiveFineTick();
    void handleLiveDrumRollThreeAxis(ControlCommand command);
    void syncLiveTrellis();
    bool isSelectedSequenceWaiting();
    SequenceDisplayState getSequenceDisplayState();
    void toggleSelectedSequenceRecording();

    void recorderNoteOn(uint8_t note, uint8_t velocity);
    void recorderNoteOff(uint8_t note);
    void recorderAdvanceTick();
    void recorderDumpToSequence();

    void saveSequencer(const String& fileName);
    void loadSequencer(const String& fileName);
    bool patternFileExists(const String& fileName);

    // Scene management
    void sceneAdd();
    void sceneRemove();
    void sceneToggleAll();
    int  getSceneCount() const;
    void presentSceneSettings(int8_t focusedPad = -1);

    // Direct trellis access for pattern bank UI
    void clearTrellis();
    void setTrellisButtonColor(uint8_t index, uint32_t color);
    void showTrellis();

    // Sequencer and music manager access
    ISequencer& getSequencer() { return _sequencer; }
    RTPSequencer& getConcreteSequencer() { return _concreteSequencer; }
    MusicManager& getMusicManager() { return _concreteSequencer.getMusicManager(); }

    // Clock generator access (set by RTPMainUnit)
    void setClockGenerator(IClockGenerator& clockGenerator) { _clockGenerator = &clockGenerator; }
    
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
    uint8_t _displayBlinkCounter = 0; // For blinking waiting indicator
    int _swingAmount = 0;               // 0-100%
    int _quantizeStrength = 50;         // 0-100%
    int _masterVolume = 100;            // 0-100%
    void writeSequenceToNeoTrellis(RTPSequenceNoteStates sequenceStates, int color);
    void writeSceneToNeoTrellis(RTPSequencesState sequencesState);
    void writeTransportPage();
};