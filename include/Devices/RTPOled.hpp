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
    String lastLines = "";

    // Deferred render: printToScreen() only records the latest requested frame
    // and marks the display dirty. flush() performs the (blocking) U8g2 page
    // loop once per loop, so a burst of updates collapses into a single redraw
    // instead of one full redraw per input event.
    struct Frame {
      String line[4];
      bool   fourLine    = false;
      bool   useState    = false;
      bool   isRecording = false;
      SequenceDisplayState state = SequenceDisplayState::Stopped;
      bool   blink       = false;
      String key;
    };
    Frame _pend;   // latest requested frame (written by _queue)
    Frame _act;    // frame currently being rendered (stable across pages)
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
    void flush();   // render the pending frame once per loop (no-op when clean)
private:
    u8g2_int_t calcOffsetToCenterText(String textLine);
    void _queue(const String& l1, const String& l2, const String& l3, const String& l4,
                bool fourLine, bool isRecording, SequenceDisplayState state, bool blink, bool useState);
    void _drawFrame();   // draw all lines into the current u8g2 page buffer (RAM only)
};