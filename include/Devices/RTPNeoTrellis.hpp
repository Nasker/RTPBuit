#pragma once

#include "Adafruit_NeoTrellis.h"
#include "Structs.h"
#include "ControlCommand.h"
#include "ColorFunctions.h"
#include "constants.h"

class RTPMainUnit;
class RTPNeoTrellis{
    static Adafruit_NeoTrellis myTrellis;
    static RTPMainUnit* mainUnit;
 public:
    RTPNeoTrellis();
    void begin(RTPMainUnit* _mainUnit);
    void introAnimation();
    static TrellisCallback blink(keyEvent evt);
    void read();
    void writeSequenceStates(RTPSequenceNoteStates seqStates, int color, bool show=true);
    void writeSceneStates(RTPSequencesState sceneStates);
    void writeBuitCCStates(RTPSequencesState buitCCStates, int color);
    void writeSequenceSettingsPage(SequenceSettings sequenceSettings);
    void writeTransportPage(int color);
    void moveCursor(int cursorPos);
    
    // Individual button control for transport state
    void setButtonColor(int buttonIndex, uint32_t color);
    void clearButton(int buttonIndex);
    void clearAllButtons();
    void show();
    
    // Color helpers
    static uint32_t colorGreen();
    static uint32_t colorRed();
    static uint32_t colorYellow();
    static uint32_t colorBlue();
    static uint32_t colorWhite();
    static uint32_t colorOff();
    static uint32_t colorDim(uint32_t color, uint8_t brightness);
    
    // Pattern bank color helpers
    static uint32_t colorForPage(uint8_t page);           // Hue gradient across 16 pages
    static uint32_t colorForSlot(uint8_t page, bool exists); // Page hue, bright=exists, dim=empty
};