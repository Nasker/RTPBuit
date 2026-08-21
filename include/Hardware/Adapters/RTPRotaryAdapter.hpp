#pragma once

#include "Interfaces/IInputDevice.hpp"
#include "Devices/RTPRotaryClickChordion.hpp"
#include <functional>

/**
 * @brief Adapter to make RTPRotaryClickDev compatible with IRotaryEncoder interface
 * 
 * This adapter wraps the existing RTPRotaryClickDev class and implements the IRotaryEncoder
 * interface, allowing it to be used with the new decomposed managers.
 */
class RTPRotaryAdapter : public IRotaryEncoder {
private:
    RTPRotaryClickDev& _rotary;
    bool _ready;
    std::function<void(bool)> _rotationCallback;
    std::function<void(int)> _clickCallback;

public:
    /**
     * @brief Constructor
     * @param rotary Reference to existing RTPRotaryClickDev instance
     */
    explicit RTPRotaryAdapter(RTPRotaryClickDev& rotary) 
        : _rotary(rotary), _ready(false) {}

    bool initialize() override {
        // RTPRotaryClickDev doesn't require explicit initialization
        _ready = true;
        return true;
    }

    void update() override {
        // RTPRotaryClickDev is updated via callbackFromRotary/callbackFromClicks in RTPMainUnit
    }

    bool isReady() const override {
        return _ready;
    }

    void setRotationCallback(std::function<void(bool)> callback) override {
        _rotationCallback = callback;
    }

    void setClickCallback(std::function<void(int)> callback) override {
        _clickCallback = callback;
    }

    bool isClockwise() const override {
        // Would need to track last rotation direction
        return true; // Placeholder
    }
};
