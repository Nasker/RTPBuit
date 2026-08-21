#include "Managers/InputManager.hpp"
#include "Error/Logger.hpp"
#include "Error/ErrorHandler.hpp"
#include "Config/UiConfig.hpp"
#include "Hardware/Implementations/NeoTrellisMatrix.hpp"
#include <memory>

InputManager::InputManager()
    : _buttonMatrix(nullptr), _rotaryEncoder(nullptr), _threeAxisSensor(nullptr),
      _ready(false), _lastErrorTime(0) {
}

InputManager::InputManager(std::shared_ptr<IButtonMatrix> buttonMatrix,
                           std::shared_ptr<IRotaryEncoder> rotaryEncoder,
                           std::shared_ptr<IThreeAxisSensor> threeAxisSensor)
    : _buttonMatrix(buttonMatrix), _rotaryEncoder(rotaryEncoder), _threeAxisSensor(threeAxisSensor),
      _ready(false), _lastErrorTime(0) {
    
    if (!_buttonMatrix || !_rotaryEncoder || !_threeAxisSensor) {
        ErrorHandler::handleError(ErrorCode::InvalidPointer, ErrorSeverity::Critical,
                                 "Input device interfaces cannot be null", "InputManager");
    }
}

Result<void> InputManager::initialize() {
    if (!_buttonMatrix || !_rotaryEncoder || !_threeAxisSensor) {
        return Result<void>::failure(ErrorCode::InvalidPointer, ErrorSeverity::Critical,
                                  "Input device interfaces not available");
    }

    // Initialize button matrix
    if (!_buttonMatrix->initialize()) {
        logError("Failed to initialize button matrix");
        return Result<void>::failure(ErrorCode::ButtonMatrixInitFailed, ErrorSeverity::Error,
                                  "Button matrix initialization failed");
    }

    // Set up button callbacks
    _buttonMatrix->setPressCallback([this](uint8_t button) { onButtonPressed(button); });
    _buttonMatrix->setReleaseCallback([this](uint8_t button) { onButtonReleased(button); });

    // Initialize rotary encoder
    if (!_rotaryEncoder->initialize()) {
        logError("Failed to initialize rotary encoder");
        return Result<void>::failure(ErrorCode::RotaryEncoderInitFailed, ErrorSeverity::Error,
                                  "Rotary encoder initialization failed");
    }

    // Set up rotary callback
    _rotaryEncoder->setRotationCallback([this](bool clockwise) { onRotaryTurned(clockwise); });

    // Initialize three-axis sensor
    if (!_threeAxisSensor->initialize()) {
        logError("Failed to initialize three-axis sensor");
        return Result<void>::failure(ErrorCode::SensorNotFound, ErrorSeverity::Error,
                                  "Three-axis sensor initialization failed");
    }

    // Set up three-axis callback
    _threeAxisSensor->setChangeCallback([this](int left, int center, int right) { 
        onThreeAxisChanged(left, center, right); 
    });

    _ready = true;
    LOG_INFO_CTX("InputManager initialized successfully", "InputManager");
    return Result<void>::Ok();
}

bool InputManager::isReady() const {
    return _ready && 
           _buttonMatrix && _buttonMatrix->isReady() &&
           _rotaryEncoder && _rotaryEncoder->isReady() &&
           _threeAxisSensor && _threeAxisSensor->isReady();
}

void InputManager::update() {
    if (!isReady()) return;
    
    _buttonMatrix->update();
    _rotaryEncoder->update();
    _threeAxisSensor->update();
}

void InputManager::poll() {
    update(); // For now, poll is same as update
}

void InputManager::setButtonPressCallback(std::function<void(uint8_t)> callback) {
    _buttonPressCallback = callback;
}

void InputManager::setButtonReleaseCallback(std::function<void(uint8_t)> callback) {
    _buttonReleaseCallback = callback;
}

void InputManager::setButtonColor(uint8_t button, uint32_t color) {
    if (!isReady()) return;
    validateButtonIndex(button);
    _buttonMatrix->setButtonColor(button, color);
}

void InputManager::clearButton(uint8_t button) {
    if (!isReady()) return;
    validateButtonIndex(button);
    _buttonMatrix->clearButton(button);
}

void InputManager::clearAllButtons() {
    if (!isReady()) return;
    _buttonMatrix->clearAllButtons();
}

void InputManager::showButtons() {
    if (!isReady()) return;
    _buttonMatrix->show();
}

void InputManager::writeSequenceStates(const bool states[16], int color) {
    if (!isReady()) return;
    
    // Cast to specific implementation if available
    auto neoTrellis = std::static_pointer_cast<NeoTrellisMatrix>(_buttonMatrix);
    if (neoTrellis) {
        neoTrellis->writeSequenceStates(states, color);
    }
}

void InputManager::writeSceneStates(const bool states[16]) {
    if (!isReady()) return;
    
    auto neoTrellis = std::static_pointer_cast<NeoTrellisMatrix>(_buttonMatrix);
    if (neoTrellis) {
        neoTrellis->writeSceneStates(states);
    }
}

void InputManager::writeTransportPage() {
    if (!isReady()) return;
    
    auto neoTrellis = std::static_pointer_cast<NeoTrellisMatrix>(_buttonMatrix);
    if (neoTrellis) {
        neoTrellis->writeTransportPage();
    }
}

void InputManager::introAnimation() {
    if (!isReady()) return;
    
    auto neoTrellis = std::static_pointer_cast<NeoTrellisMatrix>(_buttonMatrix);
    if (neoTrellis) {
        neoTrellis->introAnimation();
    }
}

void InputManager::setRotaryCallback(std::function<void(bool clockwise)> callback) {
    _rotaryCallback = callback;
}

bool InputManager::getRotaryDirection() const {
    if (!isReady()) return false;
    return _rotaryEncoder->isClockwise();
}

void InputManager::setThreeAxisCallback(std::function<void(int left, int center, int right)> callback) {
    _threeAxisCallback = callback;
}

ThreeReadings InputManager::getCurrentReadings() const {
    if (!isReady()) return {0, 0, 0};
    return _threeAxisSensor->getCurrentReadings();
}

ThreeReadings InputManager::getCleanReadings() const {
    if (!isReady()) return {0, 0, 0};
    return _threeAxisSensor->getCleanReadings();
}

void InputManager::setSensorLimits(int minReading, int maxReading) {
    if (!isReady()) return;
    _threeAxisSensor->setMinReading(minReading);
    _threeAxisSensor->setMaxReading(maxReading);
}

uint8_t InputManager::getButtonCount() const {
    if (!isReady()) return 0;
    return _buttonMatrix->getButtonCount();
}

bool InputManager::isButtonPressed(uint8_t button) const {
    if (!isReady()) return false;
    validateButtonIndex(button);
    // This would depend on the specific implementation
    return false; // Placeholder
}

uint32_t InputManager::getTimeSinceLastInput() const {
    return millis() - _lastErrorTime;
}

void InputManager::logError(const String& message) const {
    LOG_ERROR_CTX(message, "InputManager");
    _lastErrorTime = millis();
}

void InputManager::onButtonPressed(uint8_t button) {
    LOG_DEBUG_CTX("Button pressed: " + String(button), "InputManager");
    if (_buttonPressCallback) {
        _buttonPressCallback(button);
    }
}

void InputManager::onButtonReleased(uint8_t button) {
    LOG_DEBUG_CTX("Button released: " + String(button), "InputManager");
    if (_buttonReleaseCallback) {
        _buttonReleaseCallback(button);
    }
}

void InputManager::onRotaryTurned(bool clockwise) {
    LOG_DEBUG_CTX("Rotary turned: " + String(clockwise ? "CW" : "CCW"), "InputManager");
    if (_rotaryCallback) {
        _rotaryCallback(clockwise);
    }
}

void InputManager::onThreeAxisChanged(int left, int center, int right) {
    LOG_DEBUG_CTX("Three-axis: L=" + String(left) + " C=" + String(center) + " R=" + String(right), 
                  "InputManager");
    if (_threeAxisCallback) {
        _threeAxisCallback(left, center, right);
    }
}

void InputManager::validateButtonIndex(uint8_t button) const {
    if (button >= getButtonCount()) {
        logError("Invalid button index: " + String(button));
    }
}
