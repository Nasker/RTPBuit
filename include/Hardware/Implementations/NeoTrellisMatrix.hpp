#pragma once

#include "Interfaces/IInputDevice.hpp"
#include "Config/HardwareConfig.hpp"
#include "Adafruit_NeoTrellis.h"
#include <functional>

/**
 * @brief NeoTrellis 4x4 button matrix implementation
 * 
 * Implements the IButtonMatrix interface for the Adafruit NeoTrellis 4x4 LED button matrix.
 * Provides button press/release events and individual LED control.
 */
class NeoTrellisMatrix : public IButtonMatrix {
private:
    static Adafruit_NeoTrellis _trellis;
    static NeoTrellisMatrix* _instance;
    
    std::function<void(uint8_t)> _pressCallback;
    std::function<void(uint8_t)> _releaseCallback;
    bool _ready;
    uint32_t _lastErrorTime;
    uint32_t _buttonStates[16];  // For debouncing and state tracking

public:
    /**
     * @brief Constructor
     */
    NeoTrellisMatrix();

    // IInputDevice interface
    bool initialize() override;
    void update() override;
    bool isReady() const override;

    // IButtonMatrix interface
    void setPressCallback(std::function<void(uint8_t button)> callback) override;
    void setReleaseCallback(std::function<void(uint8_t button)> callback) override;
    void setButtonColor(uint8_t button, uint32_t color) override;
    void clearButton(uint8_t button) override;
    void clearAllButtons() override;
    void show() override;
    uint8_t getButtonCount() const override;

    // Additional methods
    void introAnimation();
    void writeSequenceStates(const bool states[16], int color);
    void writeSceneStates(const bool states[16]);
    void writeTransportPage();
    void moveCursor(int cursorPos);

    // Color utilities
    static uint32_t colorGreen();
    static uint32_t colorRed();
    static uint32_t colorYellow();
    static uint32_t colorBlue();
    static uint32_t colorWhite();
    static uint32_t colorOff();
    static uint32_t colorDim(uint32_t color, uint8_t brightness);
    static uint32_t colorForPage(uint8_t page);
    static uint32_t colorForSlot(uint8_t page, bool exists);

private:
    static TrellisCallback staticCallback(keyEvent evt);
    void handleKeyEvent(keyEvent evt);
    bool validateButton(uint8_t button);
    void logError(const String& message);
    bool isButtonPressed(uint8_t button);
    void setButtonState(uint8_t button, bool pressed);
};
