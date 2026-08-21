#include "Managers/DeviceManager.hpp"
#include "Error/Logger.hpp"
#include "Error/ErrorHandler.hpp"
#include <memory>

DeviceManager::DeviceManager(std::shared_ptr<IDisplay> display,
                             std::shared_ptr<IButtonMatrix> buttonMatrix,
                             std::shared_ptr<IRotaryEncoder> rotaryEncoder,
                             std::shared_ptr<IThreeAxisSensor> threeAxisSensor,
                             std::shared_ptr<IClockGenerator> clockGenerator,
                             std::shared_ptr<ISequencer> sequencer,
                             std::shared_ptr<IMidiOutput> midiOutput)
    : _midiOutput(midiOutput), _ready(false), _lastErrorTime(0) {
    
    // Create focused managers
    _displayManager = std::make_unique<DisplayManager>(display);
    _inputManager = std::make_unique<InputManager>(buttonMatrix, rotaryEncoder, threeAxisSensor);
    _transportManager = std::make_unique<TransportManager>(clockGenerator, sequencer);
    
    if (!_displayManager || !_inputManager || !_transportManager || !_midiOutput) {
        ErrorHandler::handleError(ErrorCode::InvalidPointer, ErrorSeverity::Critical,
                                 "Device manager components cannot be null", "DeviceManager");
    }
}

Result<void> DeviceManager::initialize() {
    if (!_displayManager || !_inputManager || !_transportManager || !_midiOutput) {
        return Result<void>::failure(ErrorCode::InvalidPointer, ErrorSeverity::Critical,
                                  "Device manager components not available");
    }

    // Initialize all managers
    auto displayResult = _displayManager->initialize();
    if (displayResult.isError()) {
        return displayResult;
    }

    auto inputResult = _inputManager->initialize();
    if (inputResult.isError()) {
        return inputResult;
    }

    auto transportResult = _transportManager->initialize();
    if (transportResult.isError()) {
        return transportResult;
    }

    // Setup event routing between managers
    setupEventRouting();

    _ready = true;
    LOG_INFO_CTX("DeviceManager initialized successfully", "DeviceManager");
    return Result<void>::Ok();
}

bool DeviceManager::isReady() const {
    return _ready && 
           _displayManager && _displayManager->isReady() &&
           _inputManager && _inputManager->isReady() &&
           _transportManager && _transportManager->isReady();
}

void DeviceManager::update() {
    if (!isReady()) return;
    
    _inputManager->update();
    // Other managers update as needed
}

void DeviceManager::poll() {
    update();
}

void DeviceManager::printFourLines(const String& line1, const String& line2, const String& line3, const String& line4) {
    if (_displayManager) {
        _displayManager->printFourLines(line1, line2, line3, line4);
    }
}

void DeviceManager::displayTransportState() {
    if (!_displayManager || !_transportManager) return;
    
    auto state = _transportManager->getState();
    _displayManager->displayTransportPage(state.isPlaying, state.isRecording, 
                                         state.isWaiting, state.syncMode);
}

void DeviceManager::displayError(const String& errorMessage) {
    if (_displayManager) {
        _displayManager->displayError(errorMessage);
    }
}

void DeviceManager::showIntroAnimation() {
    if (_displayManager) {
        _displayManager->showIntroAnimation("I'm RTP's BUIT!!");
    }
}

void DeviceManager::setButtonPressCallback(std::function<void(uint8_t)> callback) {
    if (_inputManager) {
        _inputManager->setButtonPressCallback(callback);
    }
}

void DeviceManager::setRotaryCallback(std::function<void(bool)> callback) {
    if (_inputManager) {
        _inputManager->setRotaryCallback(callback);
    }
}

void DeviceManager::setThreeAxisCallback(std::function<void(int, int, int)> callback) {
    if (_inputManager) {
        _inputManager->setThreeAxisCallback(callback);
    }
}

void DeviceManager::writeTransportPage() {
    if (_inputManager) {
        _inputManager->writeTransportPage();
    }
}

void DeviceManager::play() {
    if (_transportManager) {
        _transportManager->play();
        displayTransportState(); // Update display
    }
}

void DeviceManager::stop() {
    if (_transportManager) {
        _transportManager->stop();
        displayTransportState(); // Update display
    }
}

void DeviceManager::togglePlayback() {
    if (_transportManager) {
        _transportManager->togglePlayback();
        displayTransportState(); // Update display
    }
}

void DeviceManager::toggleRecording() {
    if (_transportManager) {
        _transportManager->toggleRecording();
        displayTransportState(); // Update display
    }
}

void DeviceManager::setBPM(float bpm) {
    if (_transportManager) {
        _transportManager->setBPM(bpm);
        displayTransportState(); // Update display
    }
}

void DeviceManager::tapTempo() {
    if (_transportManager) {
        _transportManager->tapTempo();
        displayTransportState(); // Update display
    }
}

void DeviceManager::reset() {
    if (_transportManager) {
        _transportManager->reset();
    }
    
    if (_inputManager) {
        _inputManager->clearAllButtons();
        _inputManager->showButtons();
    }
    
    if (_displayManager) {
        _displayManager->clear();
        displayTransportState();
    }
    
    LOG_INFO_CTX("DeviceManager reset", "DeviceManager");
}

void DeviceManager::shutdown() {
    if (_transportManager) {
        _transportManager->stop();
    }
    
    if (_displayManager) {
        _displayManager->clear();
        _displayManager->printFourLines("SHUTTING DOWN", "Goodbye!", "", "");
    }
    
    if (_inputManager) {
        _inputManager->clearAllButtons();
        _inputManager->showButtons();
    }
    
    _ready = false;
    LOG_INFO_CTX("DeviceManager shutdown", "DeviceManager");
}

String DeviceManager::getStatusString() const {
    if (!isReady()) return "NOT READY";
    
    String status = "DeviceManager: ";
    if (_transportManager) {
        status += _transportManager->getStatusString();
    }
    
    return status;
}

DeviceManager::SystemStatus DeviceManager::getSystemStatus() const {
    return {
        .displayReady = _displayManager && _displayManager->isReady(),
        .inputReady = _inputManager && _inputManager->isReady(),
        .transportReady = _transportManager && _transportManager->isReady(),
        .midiReady = _midiOutput && _midiOutput->isReady(),
        .overallReady = isReady(),
        .lastError = "", // Would need to track last error
        .uptimeMs = millis()
    };
}

void DeviceManager::logError(const String& message) {
    LOG_ERROR_CTX(message, "DeviceManager");
    _lastErrorTime = millis();
}

Result<void> DeviceManager::initializeManagers() {
    // This is handled in initialize() method
    return Result<void>::Ok();
}

void DeviceManager::setupEventRouting() {
    // Setup event routing between managers
    // For example, transport state changes update display
    
    if (_transportManager && _displayManager) {
        _transportManager->setPlaybackChangeCallback([this](bool isPlaying) {
            displayTransportState();
        });
        
        _transportManager->setRecordingChangeCallback([this](bool isRecording, bool isWaiting) {
            displayTransportState();
        });
        
        _transportManager->setTempoChangeCallback([this](float bpm) {
            displayTransportState();
        });
    }
    
    // Setup input callbacks to handle transport controls
    if (_inputManager) {
        _inputManager->setButtonPressCallback([this](uint8_t button) {
            // Handle transport button presses
            switch (button) {
                case 0: // Play button
                    togglePlayback();
                    break;
                case 1: // Stop button
                    stop();
                    break;
                case 2: // Record button
                    toggleRecording();
                    break;
                case 3: // Tap tempo button
                    tapTempo();
                    break;
                default:
                    // Other buttons handled by application
                    break;
            }
        });
    }
}
