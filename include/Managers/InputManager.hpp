#pragma once

#include "Interfaces/IInputDevice.hpp"
#include "Config/HardwareConfig.hpp"
#include "Error/Result.hpp"
#include <Arduino.h>
#include <functional>
#include <memory>

/**
 * @brief Input device manager
 * 
 * Extracted from BuitDevicesManager to follow Single Responsibility Principle.
 * Handles all input device operations including button matrix, rotary encoder,
 * and three-axis sensors with proper debouncing and event routing.
 */
class InputManager {
private:
    std::shared_ptr<IButtonMatrix> _buttonMatrix;
    std::shared_ptr<IRotaryEncoder> _rotaryEncoder;
    std::shared_ptr<IThreeAxisSensor> _threeAxisSensor;
    
    bool _ready;
    mutable uint32_t _lastErrorTime;
    
    // Event callbacks
    std::function<void(uint8_t)> _buttonPressCallback;
    std::function<void(uint8_t)> _buttonReleaseCallback;
    std::function<void(bool)> _rotaryCallback;
    std::function<void(int, int, int)> _threeAxisCallback;

public:
    /**
     * @brief Default constructor
     */
    InputManager();
    
    /**
     * @brief Constructor
     * @param buttonMatrix Button matrix interface
     * @param rotaryEncoder Rotary encoder interface  
     * @param threeAxisSensor Three-axis sensor interface
     */
    InputManager(std::shared_ptr<IButtonMatrix> buttonMatrix,
                 std::shared_ptr<IRotaryEncoder> rotaryEncoder,
                 std::shared_ptr<IThreeAxisSensor> threeAxisSensor);

    /**
     * @brief Initialize the input manager
     * @return true if initialization successful
     */
    Result<void> initialize();

    /**
     * @brief Check if input manager is ready
     * @return true if ready
     */
    bool isReady() const;

    // Update Operations
    void update();
    void poll();

    // Button Matrix Operations
    void setButtonPressCallback(std::function<void(uint8_t)> callback);
    void setButtonReleaseCallback(std::function<void(uint8_t)> callback);
    void setButtonColor(uint8_t button, uint32_t color);
    void clearButton(uint8_t button);
    void clearAllButtons();
    void showButtons();
    void writeSequenceStates(const bool states[16], int color);
    void writeSceneStates(const bool states[16]);
    void writeTransportPage();
    void introAnimation();

    // Rotary Encoder Operations
    void setRotaryCallback(std::function<void(bool clockwise)> callback);
    bool getRotaryDirection() const;

    // Three-Axis Sensor Operations
    void setThreeAxisCallback(std::function<void(int left, int center, int right)> callback);
    struct ThreeReadings getCurrentReadings() const;
    struct ThreeReadings getCleanReadings() const;
    void setSensorLimits(int minReading, int maxReading);

    // Utility Methods
    uint8_t getButtonCount() const;
    bool isButtonPressed(uint8_t button) const;
    uint32_t getTimeSinceLastInput() const;

private:
    void logError(const String& message) const;
    void onButtonPressed(uint8_t button);
    void onButtonReleased(uint8_t button);
    void onRotaryTurned(bool clockwise);
    void onThreeAxisChanged(int left, int center, int right);
    void validateButtonIndex(uint8_t button) const;
};
