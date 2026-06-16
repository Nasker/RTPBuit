#pragma once

#include "Interfaces/IDisplay.hpp"
#include "Config/UiConfig.hpp"
#include "Error/Result.hpp"
#include "Interfaces/IClockGenerator.hpp"
#include <Arduino.h>
#include <WString.h>
#include <memory>

/**
 * @brief Display operations manager
 * 
 * Extracted from BuitDevicesManager to follow Single Responsibility Principle.
 * Handles all display-related operations including multi-line text, recording indicators,
 * and UI state visualization.
 */
class DisplayManager {
private:
    std::shared_ptr<IDisplay> _display;
    bool _ready;
    uint32_t _lastErrorTime;

public:
    /**
     * @brief Default constructor
     */
    DisplayManager();
    
    /**
     * @brief Constructor
     * @param display Display interface implementation
     */
    explicit DisplayManager(std::shared_ptr<IDisplay> display);

    /**
     * @brief Initialize the display manager
     * @return true if initialization successful
     */
    Result<void> initialize();

    /**
     * @brief Check if display manager is ready
     * @return true if ready
     */
    bool isReady() const { return _ready && _display && _display->isReady(); }

    // Text Display Operations
    void printThreeLines(const String& line1, const String& line2, const String& line3);
    void printFourLines(const String& line1, const String& line2, const String& line3, const String& line4);
    void printFourLinesWithRecording(const String& line1, const String& line2, const String& line3, const String& line4, bool isRecording);
    void printControlCommand(const String& firstLine, const String& secondLine, const String& thirdLine, const String& fourthLine, bool isRecording = false);

    // Animation Operations
    void showIntroAnimation(const String& text, int iterations = UiConfig::Animation::INTRO_ITERATIONS);
    void setAfterIntroMode();

    // UI State Display
    void displayTransportPage(bool isPlaying, bool isRecording, bool isWaiting, SyncMode syncMode);
    void displaySequenceInfo(const String& sequenceType, const String& sequenceName, int currentStep, int totalSteps);
    void displayParameterInfo(const String& parameterName, int parameterValue, int minValue, int maxValue);
    void displayError(const String& errorMessage);

    // Recording Indicators
    void setRecordingState(bool isRecording, bool isWaiting = false);
    void updateRecordingBlink();

    // Utility Methods
    void clear();
    void refresh();
    uint16_t getWidth() const;
    uint16_t getHeight() const;

private:
    void logError(const String& message);
    String formatProgressBar(int current, int total, int width = 20);
    String formatParameterDisplay(const String& name, int value, int min, int max);
};
