#pragma once

#include "Arduino.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "ControlCommand.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C

class RTPOled{
    Adafruit_SSD1306 display{SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, SCREEN_ADDRESS};
public:
    RTPOled();
    void init();
    void printToScreen(String firstLine, String secondLine, String thirdLine);
    void printToScreen(ControlCommand command);
};