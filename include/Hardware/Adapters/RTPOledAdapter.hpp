#pragma once

#include "Interfaces/IDisplay.hpp"
#include "Devices/RTPOled.hpp"
#include <memory>

/**
 * @brief Adapter to make RTPOled compatible with IDisplay interface
 * 
 * This adapter wraps the existing RTPOled class and implements the IDisplay
 * interface, allowing it to be used with the new decomposed managers.
 */
class RTPOledAdapter : public IDisplay {
private:
    RTPOled& _oled;
    bool _ready;

public:
    /**
     * @brief Constructor
     * @param oled Reference to existing RTPOled instance
     */
    explicit RTPOledAdapter(RTPOled& oled) : _oled(oled), _ready(false) {}

    bool initialize() override {
        _oled.init();
        _ready = true;
        return true;
    }

    void clear() override {
        // RTPOled doesn't have explicit clear, but printing empty lines works
        _oled.printToScreen("", "", "", "");
    }

    void printThreeLines(const String& line1, const String& line2, const String& line3) override {
        _oled.printToScreen(line1, line2, line3);
    }

    void printFourLines(const String& line1, const String& line2, const String& line3, const String& line4) override {
        _oled.printToScreen(line1, line2, line3, line4);
    }

    void printFourLinesWithRecording(const String& line1, const String& line2, const String& line3, const String& line4, bool isRecording) override {
        _oled.printToScreen(line1, line2, line3, line4, isRecording);
    }

    void printFourLinesWithState(const String& line1, const String& line2, const String& line3, const String& line4, SequenceDisplayState state, bool blinkState) override {
        _oled.printToScreen(line1, line2, line3, line4, state, blinkState);
    }

    void showIntroAnimation(const String& text, int iterations) override {
        // Legacy semantics: x starts at screen width and is mutated by reference
        // on each frame, producing the scroll-in effect.
        int x = SCREEN_WIDTH;
        for (int i = 0; i < iterations; i++) {
            _oled.introAnimation(x, text);
        }
    }

    void showIntroFrame(int& offset, const String& text) override {
        _oled.introAnimation(offset, text);
    }

    void setAfterIntro() override {
        _oled.setAfterIntro();
    }

    bool isReady() const override {
        return _ready;
    }

    uint16_t getWidth() const override {
        return SCREEN_WIDTH;
    }

    uint16_t getHeight() const override {
        return SCREEN_HEIGHT;
    }

    void show() override {
        // RTPOled handles display updates internally
    }
};
