#pragma once

#include "Interfaces/IDisplay.hpp"
#include "Config/HardwareConfig.hpp"
#include <U8g2lib.h>

/**
 * @brief OLED display implementation using U8G2 library
 * 
 * Implements the IDisplay interface for SH1106 OLED displays.
 * Provides text display capabilities with multiple lines and recording indicators.
 */
class OledDisplay : public IDisplay {
private:
    U8G2_SH1106_128X64_NONAME_1_HW_I2C _display;
    bool _ready;
    uint32_t _lastErrorTime;
    String _lastLines[4];

public:
    /**
     * @brief Constructor
     */
    OledDisplay();

    // IDisplay interface implementation
    bool initialize() override;
    void clear() override;
    void printThreeLines(const String& line1, const String& line2, const String& line3) override;
    void printFourLines(const String& line1, const String& line2, const String& line3, const String& line4) override;
    void printFourLinesWithRecording(const String& line1, const String& line2, const String& line3, const String& line4, bool isRecording) override;
    void showIntroAnimation(const String& text, int iterations) override;
    void setAfterIntro() override;
    bool isReady() const override;
    uint16_t getWidth() const override;
    uint16_t getHeight() const override;

    // Additional methods
    void printToScreen(const String& firstLine, const String& secondLine, const String& thirdLine);
    void printToScreen(const String& firstLine, const String& secondLine, const String& thirdLine, const String& fourthLine, bool isRecording = false);
    void printToScreen(const String& firstLine, const String& secondLine, const String& thirdLine, const String& fourthLine, bool isRecording, bool blinkState);
    void show() override;

private:
    int16_t calcOffsetToCenterText(const String& textLine);
    void drawRecordingIndicator(bool isRecording, bool blinkState = false);
    void logError(const String& message);
    void updateLastLines(const String& line1, const String& line2, const String& line3, const String& line4 = "");
};
