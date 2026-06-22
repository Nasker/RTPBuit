#pragma once

#include "Interfaces/IInputDevice.hpp"
#include "Devices/RTPNeoTrellis.hpp"
#include <functional>

/**
 * @brief Adapter to make RTPNeoTrellis compatible with IButtonMatrix interface
 * 
 * This adapter wraps the existing RTPNeoTrellis class and implements the IButtonMatrix
 * interface, allowing it to be used with the new decomposed managers.
 */
class RTPNeoTrellisAdapter : public IButtonMatrix {
private:
    RTPNeoTrellis& _trellis;
    bool _ready;
    std::function<void(uint8_t)> _pressCallback;
    std::function<void(uint8_t)> _releaseCallback;

public:
    /**
     * @brief Constructor
     * @param trellis Reference to existing RTPNeoTrellis instance
     */
    explicit RTPNeoTrellisAdapter(RTPNeoTrellis& trellis) 
        : _trellis(trellis), _ready(false) {}

    bool initialize() override {
        // RTPNeoTrellis is initialized via begin() in RTPMainUnit
        _ready = true;
        return true;
    }

    void update() override {
        _trellis.read();
    }

    bool isReady() const override {
        return _ready;
    }

    void setPressCallback(std::function<void(uint8_t)> callback) override {
        _pressCallback = callback;
    }

    void setReleaseCallback(std::function<void(uint8_t)> callback) override {
        _releaseCallback = callback;
    }

    void setButtonColor(uint8_t button, uint32_t color) override {
        _trellis.setButtonColor(button, color);
    }

    void clearButton(uint8_t button) override {
        _trellis.clearButton(button);
    }

    void clearAllButtons() override {
        _trellis.clearAllButtons();
    }

    void show() override {
        _trellis.show();
    }

    uint8_t getButtonCount() const override {
        return 16; // 4x4 matrix
    }
};
