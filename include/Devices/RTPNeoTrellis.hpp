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

    // Shadow buffer: tracks the colour last pushed to each physical pixel so we
    // only issue I2C writes for pixels that actually changed. seesaw
    // setPixelColor() performs a blocking I2C transaction per pixel, so skipping
    // redundant writes (and the redundant show()) is what keeps fast input from
    // starving the sequencer tick.
    uint32_t _pixelShadow[NEO_TRELLIS_NUM_KEYS];
    bool _pixelsDirty = false;
    void _pushPixel(uint8_t physIndex, uint32_t color); // write only if changed
    void _commit();                                     // show() only if dirty
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