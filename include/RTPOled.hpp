#pragma once

#include "Arduino.h"
#include <Wire.h>
#include <U8g2lib.h>
#include "ControlCommand.h"

#define TEXT_SIZE 2

class RTPOled{
    U8G2_SH1106_128X64_NONAME_F_HW_I2C display{U8G2_R0, /* reset=*/ U8X8_PIN_NONE};
    String lastLines = "";
public:
    RTPOled();
    void init();
    void introAnimation(int &x, String text);
    void printToScreen(String firstLine, String secondLine, String thirdLine);
    void printToScreen(ControlCommand command);
private:
    int calcOffsetToCenterText(String textLine);
};