#pragma once

#include "ControlCommand.h"
#include <Arduino.h>
#include <functional>

/**
 * @brief Structure to hold three-axis sensor readings
 */
struct ThreeReadings {
    int sensorReading1;
    int sensorReading2;
    int sensorReading3;
    
    ThreeReadings() : sensorReading1(0), sensorReading2(0), sensorReading3(0) {}
    ThreeReadings(int s1, int s2, int s3) : sensorReading1(s1), sensorReading2(s2), sensorReading3(s3) {}
};

/**
 * @brief Interface for input devices
 * 
 * This interface abstracts input device operations for buttons, rotary encoders,
 * and three-axis sensors, enabling different implementations and testing.
 */
class IInputDevice {
public:
    virtual ~IInputDevice() = default;

    /**
     * @brief Initialize the input device
     * @return true if initialization successful
     */
    virtual bool initialize() = 0;

    /**
     * @brief Update the input device (call periodically)
     */
    virtual void update() = 0;

    /**
     * @brief Check if device is ready
     * @return true if device is ready for operations
     */
    virtual bool isReady() const = 0;
};

/**
 * @brief Interface for button matrix input (like NeoTrellis)
 */
class IButtonMatrix : public IInputDevice {
public:
    /**
     * @brief Set callback for button press events
     * @param callback Function to call when button is pressed
     */
    virtual void setPressCallback(std::function<void(uint8_t button)> callback) = 0;

    /**
     * @brief Set callback for button release events
     * @param callback Function to call when button is released
     */
    virtual void setReleaseCallback(std::function<void(uint8_t button)> callback) = 0;

    /**
     * @brief Set button color
     * @param button Button index (0-15)
     * @param color RGB color value
     */
    virtual void setButtonColor(uint8_t button, uint32_t color) = 0;

    /**
     * @brief Clear button color
     * @param button Button index (0-15)
     */
    virtual void clearButton(uint8_t button) = 0;

    /**
     * @brief Clear all buttons
     */
    virtual void clearAllButtons() = 0;

    /**
     * @brief Update display (show changes)
     */
    virtual void show() = 0;

    /**
     * @brief Get number of buttons
     * @return Number of buttons (typically 16 for 4x4)
     */
    virtual uint8_t getButtonCount() const = 0;
};

/**
 * @brief Interface for rotary encoder with button
 */
class IRotaryEncoder : public IInputDevice {
public:
    /**
     * @brief Set callback for rotary rotation events
     * @param callback Function to call when encoder is rotated
     */
    virtual void setRotationCallback(std::function<void(bool clockwise)> callback) = 0;

    /**
     * @brief Set callback for button click events
     * @param callback Function to call when button is clicked
     */
    virtual void setClickCallback(std::function<void(int clickType)> callback) = 0;

    /**
     * @brief Get current rotation direction
     * @return true if clockwise, false if counter-clockwise
     */
    virtual bool isClockwise() const = 0;
};

/**
 * @brief Interface for three-axis distance sensors
 */
class IThreeAxisSensor : public IInputDevice {
public:
    /**
     * @brief Set callback for three-axis change events
     * @param callback Function to call when sensor values change
     */
    virtual void setChangeCallback(std::function<void(int left, int center, int right)> callback) = 0;

    /**
     * @brief Get current sensor readings
     * @return Struct with three sensor values
     */
    virtual struct ThreeReadings getCurrentReadings() const = 0;

    /**
     * @brief Get cleaned/filtered sensor readings
     * @return Struct with cleaned sensor values
     */
    virtual struct ThreeReadings getCleanReadings() const = 0;

    /**
     * @brief Set minimum reading limit
     * @param minReading Minimum valid reading
     */
    virtual void setMinReading(int minReading) = 0;

    /**
     * @brief Set maximum reading limit
     * @param maxReading Maximum valid reading
     */
    virtual void setMaxReading(int maxReading) = 0;
};
