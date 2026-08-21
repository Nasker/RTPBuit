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

    void writeSequenceStates(RTPSequenceNoteStates seqStates, int color, bool show = true) override {
        _trellis.writeSequenceStates(seqStates, color, show);
    }

    void writeSceneStates(RTPSequencesState sceneStates) override {
        _trellis.writeSceneStates(sceneStates);
    }

    void writeBuitCCStates(RTPSequencesState ccStates, int color) override {
        _trellis.writeBuitCCStates(ccStates, color);
    }

    void writeSequenceSettingsPage(SequenceSettings sequenceSettings) override {
        _trellis.writeSequenceSettingsPage(sequenceSettings);
    }

    void moveCursor(int cursorPos) override {
        _trellis.moveCursor(cursorPos);
    }

    void introAnimation() override {
        _trellis.introAnimation();
    }

    uint32_t getColorGreen() override { return RTPNeoTrellis::colorGreen(); }
    uint32_t getColorRed() override { return RTPNeoTrellis::colorRed(); }
    uint32_t getColorYellow() override { return RTPNeoTrellis::colorYellow(); }
    uint32_t getColorBlue() override { return RTPNeoTrellis::colorBlue(); }
    uint32_t getColorWhite() override { return RTPNeoTrellis::colorWhite(); }
    uint32_t getColorOff() override { return RTPNeoTrellis::colorOff(); }
    uint32_t getColorDim(uint32_t color, uint8_t brightness) override { return RTPNeoTrellis::colorDim(color, brightness); }
    uint32_t getColorForPage(uint8_t page) override { return RTPNeoTrellis::colorForPage(page); }
    uint32_t getColorForSlot(uint8_t page, bool exists) override { return RTPNeoTrellis::colorForSlot(page, exists); }
};
