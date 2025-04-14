#pragma once

#include "Arduino.h"
#include <Wire.h>
#include <U8g2lib.h>
#include "ControlCommand.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C
#define TEXT_SIZE 1

class RTPOled{
    
    U8G2_SH1106_128X64_NONAME_1_HW_I2C display{U8G2_R2, /* reset=*/ U8X8_PIN_NONE};
    String lastLines = "";
public:
    RTPOled();
    void init();
    void setAfterIntro();
    void introAnimation(int &x, String text);
    void printToScreen(String firstLine, String secondLine, String thirdLine);
    void printToScreen(ControlCommand command);
private:
    int calcOffsetToCenterText(String textLine);
};