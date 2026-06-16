#include "Managers/DisplayManager.hpp"
#include "Error/Logger.hpp"
#include "Error/ErrorHandler.hpp"
#include "Config/MusicConfig.hpp"
#include <memory>

DisplayManager::DisplayManager()
    : _display(nullptr), _ready(false), _lastErrorTime(0) {
}

DisplayManager::DisplayManager(std::shared_ptr<IDisplay> display) 
    : _display(display), _ready(false), _lastErrorTime(0) {
    
    if (!_display) {
        ErrorHandler::handleError(ErrorCode::InvalidPointer, ErrorSeverity::Critical,
                                 "Display interface cannot be null", "DisplayManager");
    }
}

Result<void> DisplayManager::initialize() {
    if (!_display) {
        return Result<void>::failure(ErrorCode::InvalidPointer, ErrorSeverity::Critical,
                                  "Display interface not available");
    }

    if (!_display->initialize()) {
        logError("Failed to initialize display");
        return Result<void>::failure(ErrorCode::DisplayInitFailed, ErrorSeverity::Error,
                                  "Display initialization failed");
    }

    _ready = true;
    LOG_INFO_CTX("DisplayManager initialized successfully", "DisplayManager");
    return Result<void>::Ok();
}

void DisplayManager::printThreeLines(const String& line1, const String& line2, const String& line3) {
    if (!isReady()) return;
    _display->printThreeLines(line1, line2, line3);
}

void DisplayManager::printFourLines(const String& line1, const String& line2, const String& line3, const String& line4) {
    if (!isReady()) return;
    _display->printFourLines(line1, line2, line3, line4);
}

void DisplayManager::printFourLinesWithRecording(const String& line1, const String& line2, const String& line3, const String& line4, bool isRecording) {
    if (!isReady()) return;
    _display->printFourLinesWithRecording(line1, line2, line3, line4, isRecording);
}

void DisplayManager::printControlCommand(const String& firstLine, const String& secondLine, const String& thirdLine, const String& fourthLine, bool isRecording) {
    printFourLinesWithRecording(firstLine, secondLine, thirdLine, fourthLine, isRecording);
}

void DisplayManager::showIntroAnimation(const String& text, int iterations) {
    if (!isReady()) return;
    _display->showIntroAnimation(text, iterations);
}

void DisplayManager::setAfterIntroMode() {
    if (!isReady()) return;
    _display->setAfterIntro();
}

void DisplayManager::displayTransportPage(bool isPlaying, bool isRecording, bool isWaiting, SyncMode syncMode) {
    if (!isReady()) return;

    String line1 = isPlaying ? "PLAYING" : "STOPPED";
    String line2 = isRecording ? (isWaiting ? "REC WAIT" : "RECORDING") : "READY";
    String line3 = (syncMode == SyncMode::Internal) ? "INTERNAL" : "EXTERNAL";
    String line4 = "RTPBuit v2.0";

    printFourLinesWithRecording(line1, line2, line3, line4, isRecording);
}

void DisplayManager::displaySequenceInfo(const String& sequenceType, const String& sequenceName, int currentStep, int totalSteps) {
    if (!isReady()) return;

    String line1 = sequenceType;
    String line2 = sequenceName;
    String line3 = "Step: " + String(currentStep + 1) + "/" + String(totalSteps);
    String line4 = formatProgressBar(currentStep, totalSteps);

    printFourLines(line1, line2, line3, line4);
}

void DisplayManager::displayParameterInfo(const String& parameterName, int parameterValue, int minValue, int maxValue) {
    if (!isReady()) return;

    String line1 = "PARAMETER";
    String line2 = parameterName;
    String line3 = formatParameterDisplay(parameterName, parameterValue, minValue, maxValue);
    String line4 = "Value: " + String(parameterValue);

    printFourLines(line1, line2, line3, line4);
}

void DisplayManager::displayError(const String& errorMessage) {
    if (!isReady()) return;

    String line1 = "ERROR";
    String line2 = errorMessage;
    String line3 = "Check system";
    String line4 = "Press button";

    printFourLines(line1, line2, line3, line4);
    
    LOG_ERROR_CTX("Displaying error: " + errorMessage, "DisplayManager");
}

void DisplayManager::setRecordingState(bool isRecording, bool isWaiting) {
    // This would update the recording indicator state
    // Implementation depends on specific display capabilities
    LOG_DEBUG_CTX("Recording state: " + String(isRecording ? "ON" : "OFF") + 
                  " (waiting: " + String(isWaiting ? "YES" : "NO") + ")", "DisplayManager");
}

void DisplayManager::updateRecordingBlink() {
    // Update blinking recording indicator
    // Implementation depends on timing and display capabilities
}

void DisplayManager::clear() {
    if (!isReady()) return;
    _display->clear();
}

void DisplayManager::refresh() {
    if (!isReady()) return;
    _display->show();
}

uint16_t DisplayManager::getWidth() const {
    return isReady() ? _display->getWidth() : 0;
}

uint16_t DisplayManager::getHeight() const {
    return isReady() ? _display->getHeight() : 0;
}

void DisplayManager::logError(const String& message) {
    LOG_ERROR_CTX(message, "DisplayManager");
    _lastErrorTime = millis();
}

String DisplayManager::formatProgressBar(int current, int total, int width) {
    if (total <= 0) return "[" + String(' ', width) + "]";
    
    int filled = (current * width) / total;
    String bar = "[";
    
    for (int i = 0; i < width; i++) {
        if (i < filled) {
            bar += "=";
        } else if (i == filled && i < width - 1) {
            bar += ">";
        } else {
            bar += " ";
        }
    }
    
    bar += "]";
    return bar;
}

String DisplayManager::formatParameterDisplay(const String& name, int value, int min, int max) {
    int range = max - min;
    if (range <= 0) return "0%";
    
    int percentage = ((value - min) * 100) / range;
    return String(percentage) + "%";
}
