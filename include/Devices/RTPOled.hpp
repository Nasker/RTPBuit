#pragma once

#include "Arduino.h"
#include <Wire.h>
#include <U8g2lib.h>
#include "ControlCommand.h"
#include "Structs.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C
#define TEXT_SIZE 1

class RTPOled{

    U8G2_SH1106_128X64_NONAME_1_HW_I2C display{U8G2_R2, /* reset=*/ U8X8_PIN_NONE};
    const uint8_t* _contentFont = u8g2_font_DigitalDisco_tf;  // body text font

    // Deferred render: printToScreen()/setHud() only record the latest requested
    // frame and mark the display dirty. flush() performs the (blocking) U8g2
    // page loop once per loop, so a burst of updates collapses into a single
    // redraw instead of one full redraw per input event.
    struct Frame {
      String line[4];
      bool   fourLine    = false;
      bool   useState    = false;
      bool   isRecording = false;
      SequenceDisplayState state = SequenceDisplayState::Stopped;
      bool   blink       = false;
      HudModel hud;            // transversal status strip
      PadHint hints[MAX_PAD_HINTS];  // trellis-pad legend (empty when numHints==0)
      uint8_t numHints   = 0;
      int8_t  focusedHint = -1;      // pad to highlight, or -1
      // Generic 4x4 cell grid (siblings view: sequences in a scene, scenes, ...)
      bool     showGrid  = false;
      uint16_t gridFill  = 0;        // bit i = cell i filled (enabled / has content)
      uint8_t  gridCount = 0;        // how many cells exist (<=16)
      int8_t   gridCursor= -1;       // highlighted cell, -1 = none
    };
    Frame _pend;   // latest requested frame (written by _queue / setHud)
    Frame _act;    // frame currently being rendered (stable across pages)
    Frame _shown;  // last fully-rendered frame (dedup baseline)
    bool  _dirty     = false;
    bool  _rendering = false;   // a multi-page render is in progress
public:
    RTPOled();
    void init();
    void setAfterIntro();
    void introAnimation(int &x, String text);
    void printToScreen(String firstLine, String secondLine, String thirdLine);
    void printToScreen(String firstLine, String secondLine, String thirdLine, String fourthLine, bool isRecording = false);
    void printToScreen(String firstLine, String secondLine, String thirdLine, String fourthLine, SequenceDisplayState state, bool blinkState = false);
    void printToScreen(ControlCommand command);
    void setHud(const HudModel& hud);   // update the status strip (cheap; dedup'd)
    void setPadLegend(const PadHint* hints, uint8_t count, int8_t focusedPad = -1);
    void setGrid(uint16_t fillMask, uint8_t count, int8_t highlight);
    void flush();   // render the pending frame once per loop (no-op when clean)
private:
    u8g2_int_t calcOffsetToCenterText(String textLine);
    void _queue(const String& l1, const String& l2, const String& l3, const String& l4,
                bool fourLine, bool isRecording, SequenceDisplayState state, bool blink, bool useState);
    void _refreshDirty();                       // _dirty = (_pend != _shown)
    bool _framesEqual(const Frame& a, const Frame& b) const;
    void _drawFrame();        // draw status strip + content into the page buffer (RAM only)
    void _drawStatusStrip();  // transversal top strip from _act.hud
    void _drawPadLegend();    // mini grid echoing trellis-pad functions
    void _drawCellRow();      // single centered row of cells (position/context)
};