#pragma once

#include <Arduino.h>
#include "Interfaces/IDisplay.hpp"

/**
 * @brief Mock display for unit testing
 * 
 * Records display calls for assertion in tests.
 */
class MockDisplay : public IDisplay {
public:
    String lastLine1, lastLine2, lastLine3, lastLine4;
    bool initialized = false;
    bool cleared = false;
    uint8_t printCallCount = 0;

    bool initialize() override {
        initialized = true;
        return true;
    }

    void printThreeLines(const String& l1, const String& l2, const String& l3) override {
        lastLine1 = l1; lastLine2 = l2; lastLine3 = l3;
        printCallCount++;
    }

    void printFourLines(const String& l1, const String& l2, const String& l3, const String& l4) override {
        lastLine1 = l1; lastLine2 = l2; lastLine3 = l3; lastLine4 = l4;
        printCallCount++;
    }

    void clear() override { cleared = true; }

    void show() override {}

    void showIntroAnimation(const String&, int) override {}

    void reset() {
        lastLine1 = lastLine2 = lastLine3 = lastLine4 = "";
        cleared = false;
        printCallCount = 0;
    }
};
