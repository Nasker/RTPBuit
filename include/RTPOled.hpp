#pragma once

#include "Arduino.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>
#include "ControlCommand.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C
#define TEXT_SIZE 2
#define OLED_RESET 4

class RTPOled{
    
    Adafruit_SH1106 display{OLED_RESET};
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