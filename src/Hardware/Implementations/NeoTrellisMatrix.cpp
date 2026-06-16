#include "Hardware/Implementations/NeoTrellisMatrix.hpp"
#include "Error/Logger.hpp"
#include "Config/UiConfig.hpp"
#include "Config/MusicConfig.hpp"

// Static member definitions
Adafruit_NeoTrellis NeoTrellisMatrix::_trellis;
NeoTrellisMatrix* NeoTrellisMatrix::_instance = nullptr;

NeoTrellisMatrix::NeoTrellisMatrix() : _ready(false), _lastErrorTime(0) {
    _instance = this;
    for (int i = 0; i < 16; i++) {
        _buttonStates[i] = 0;
    }
}

bool NeoTrellisMatrix::initialize() {
    if (!_trellis.begin()) {
        logError("Failed to initialize NeoTrellis");
        return false;
    }
    
    // Set up callback
    // Register callback for each button
    for (uint8_t i = 0; i < NEO_TRELLIS_NUM_KEYS; i++) {
        _trellis.registerCallback(i, staticCallback);
    }
    
    _ready = true;
    LOG_INFO_CTX("NeoTrellis initialized", "NeoTrellisMatrix");
    return true;
}

void NeoTrellisMatrix::update() {
    if (!_ready) return;
    _trellis.read();
}

bool NeoTrellisMatrix::isReady() const {
    return _ready;
}

void NeoTrellisMatrix::setPressCallback(std::function<void(uint8_t button)> callback) {
    _pressCallback = callback;
}

void NeoTrellisMatrix::setReleaseCallback(std::function<void(uint8_t button)> callback) {
    _releaseCallback = callback;
}

void NeoTrellisMatrix::setButtonColor(uint8_t button, uint32_t color) {
    if (!validateButton(button)) return;
    _trellis.pixels.setPixelColor(button, color);
}

void NeoTrellisMatrix::clearButton(uint8_t button) {
    if (!validateButton(button)) return;
    _trellis.pixels.setPixelColor(button, colorOff());
}

void NeoTrellisMatrix::clearAllButtons() {
    for (uint8_t i = 0; i < getButtonCount(); i++) {
        clearButton(i);
    }
}

void NeoTrellisMatrix::show() {
    if (!_ready) return;
    _trellis.pixels.show();
}

uint8_t NeoTrellisMatrix::getButtonCount() const {
    return HardwareConfig::ButtonMatrix::GRID_SIZE;
}

void NeoTrellisMatrix::introAnimation() {
    if (!_ready) return;
    
    for (int i = 0; i < UiConfig::Animation::INTRO_ITERATIONS; i++) {
        for (uint8_t button = 0; button < getButtonCount(); button++) {
            uint32_t color = colorWhite();
            if (i % 2 == 0) {
                color = colorDim(color, UiConfig::Colors::DIM_BRIGHTNESS);
            }
            setButtonColor(button, color);
        }
        show();
        delay(50);
    }
}

void NeoTrellisMatrix::writeSequenceStates(const bool states[16], int color) {
    if (!_ready) return;
    
    for (uint8_t i = 0; i < getButtonCount(); i++) {
        if (states[i]) {
            setButtonColor(i, color);
        } else {
            clearButton(i);
        }
    }
    show();
}

void NeoTrellisMatrix::writeSceneStates(const bool states[16]) {
    if (!_ready) return;
    
    for (uint8_t i = 0; i < getButtonCount(); i++) {
        if (states[i]) {
            setButtonColor(i, UiConfig::Colors::ACTIVE_COLOR);
        } else {
            setButtonColor(i, UiConfig::Colors::INACTIVE_COLOR);
        }
    }
    show();
}

void NeoTrellisMatrix::writeTransportPage() {
    if (!_ready) return;
    
    clearAllButtons();
    
    // Play button
    setButtonColor(UiConfig::Transport::PLAY_BUTTON, UiConfig::Colors::PLAY_COLOR);
    
    // Stop button  
    setButtonColor(UiConfig::Transport::STOP_BUTTON, UiConfig::Colors::STOP_COLOR);
    
    // Rec button
    setButtonColor(UiConfig::Transport::REC_BUTTON, UiConfig::Colors::REC_COLOR);
    
    // Tap button
    setButtonColor(UiConfig::Transport::TAP_BUTTON, UiConfig::Colors::TAP_COLOR);
    
    // Mode button
    setButtonColor(UiConfig::Transport::MODE_BUTTON, UiConfig::Colors::MODE_COLOR);
    
    show();
}

void NeoTrellisMatrix::moveCursor(int cursorPos) {
    if (!validateButton(cursorPos)) return;
    
    // Blink effect for cursor
    static bool blinkState = false;
    blinkState = !blinkState;
    
    if (blinkState) {
        setButtonColor(cursorPos, colorWhite());
    } else {
        setButtonColor(cursorPos, colorDim(colorWhite(), UiConfig::Colors::DIM_BRIGHTNESS));
    }
    show();
}

// Static color methods
uint32_t NeoTrellisMatrix::colorGreen() { return UiConfig::Colors::PLAY_COLOR; }
uint32_t NeoTrellisMatrix::colorRed() { return UiConfig::Colors::REC_COLOR; }
uint32_t NeoTrellisMatrix::colorYellow() { return UiConfig::Colors::TAP_COLOR; }
uint32_t NeoTrellisMatrix::colorBlue() { return UiConfig::Colors::STOP_COLOR; }
uint32_t NeoTrellisMatrix::colorWhite() { return UiConfig::Colors::ACTIVE_COLOR; }
uint32_t NeoTrellisMatrix::colorOff() { return MusicConfig::Colors::COLOR_OFF; }

uint32_t NeoTrellisMatrix::colorDim(uint32_t color, uint8_t brightness) {
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;
    
    r = (r * brightness) / 255;
    g = (g * brightness) / 255;
    b = (b * brightness) / 255;
    
    return (r << 16) | (g << 8) | b;
}

uint32_t NeoTrellisMatrix::colorForPage(uint8_t page) {
    // Create hue gradient across 16 pages
    uint16_t hue = (page * UiConfig::PatternBank::PAGE_HUE_STEPS) % 360;
    
    // Simple HSV to RGB conversion (simplified)
    uint8_t r, g, b;
    if (hue < 120) {
        r = (120 - hue) * 2;
        g = hue * 2;
        b = 0;
    } else if (hue < 240) {
        r = 0;
        g = (240 - hue) * 2;
        b = (hue - 120) * 2;
    } else {
        r = (hue - 240) * 2;
        g = 0;
        b = (360 - hue) * 2;
    }
    
    return (r << 16) | (g << 8) | b;
}

uint32_t NeoTrellisMatrix::colorForSlot(uint8_t page, bool exists) {
    uint32_t baseColor = colorForPage(page);
    if (exists) {
        return baseColor;
    } else {
        return colorDim(baseColor, UiConfig::Colors::VERY_DIM_BRIGHTNESS);
    }
}

// Static callback handler
TrellisCallback NeoTrellisMatrix::staticCallback(keyEvent evt) {
    if (_instance) {
        _instance->handleKeyEvent(evt);
    }
    return nullptr; // Return required callback
}

void NeoTrellisMatrix::handleKeyEvent(keyEvent evt) {
    uint8_t button = evt.bit.NUM;
    
    if (!validateButton(button)) return;
    
    // Debounce check
    uint32_t currentTime = millis();
    if (currentTime - _buttonStates[button] < HardwareConfig::Timing::DEBOUNCE_MS) {
        return;
    }
    
    if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_RISING) {
        // Button pressed
        _buttonStates[button] = currentTime;
        if (_pressCallback) {
            _pressCallback(button);
        }
        LOG_DEBUG_CTX("Button pressed: " + String(button), "NeoTrellisMatrix");
    } else if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_FALLING) {
        // Button released
        if (_releaseCallback) {
            _releaseCallback(button);
        }
        LOG_DEBUG_CTX("Button released: " + String(button), "NeoTrellisMatrix");
    }
}

bool NeoTrellisMatrix::validateButton(uint8_t button) {
    if (button >= getButtonCount()) {
        logError("Invalid button index: " + String(button));
        _lastErrorTime = millis();
        return false;
    }
    return true;
}

void NeoTrellisMatrix::logError(const String& message) {
    LOG_ERROR_CTX(message, "NeoTrellisMatrix");
    _lastErrorTime = millis();
}

bool NeoTrellisMatrix::isButtonPressed(uint8_t button) {
    if (!validateButton(button)) return false;
    // Check if button is currently pressed by reading its state
    // Adafruit_NeoTrellis doesn't have isPressed, so we'll track state internally
    return _buttonStates[button] > 0;
}

void NeoTrellisMatrix::setButtonState(uint8_t button, bool pressed) {
    if (!validateButton(button)) return;
    _buttonStates[button] = pressed ? millis() : 0;
}
