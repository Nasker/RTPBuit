#pragma once

#include <cstdint>
#include <stddef.h>
#include <Arduino.h>
#include <WString.h>

/**
 * @brief Interface for display output abstraction
 * 
 * This interface abstracts display operations, allowing for different
 * display implementations (OLED, LCD, etc.) and enabling proper testing.
 */
class IDisplay {
public:
    virtual ~IDisplay() = default;

    /**
     * @brief Initialize the display
     * @return true if initialization successful
     */
    virtual bool initialize() = 0;

    /**
     * @brief Clear the display
     */
    virtual void clear() = 0;

    /**
     * @brief Display three lines of text
     * @param line1 First line of text
     * @param line2 Second line of text
     * @param line3 Third line of text
     */
    virtual void printThreeLines(const String& line1, const String& line2, const String& line3) = 0;

    /**
     * @brief Display four lines of text
     * @param line1 First line of text
     * @param line2 Second line of text
     * @param line3 Third line of text
     * @param line4 Fourth line of text
     */
    virtual void printFourLines(const String& line1, const String& line2, const String& line3, const String& line4) = 0;

    /**
     * @brief Display four lines with recording indicator
     * @param line1 First line of text
     * @param line2 Second line of text
     * @param line3 Third line of text
     * @param line4 Fourth line of text
     * @param isRecording Show recording indicator
     */
    virtual void printFourLinesWithRecording(const String& line1, const String& line2, const String& line3, const String& line4, bool isRecording) = 0;

    /**
     * @brief Show intro animation with scrolling text
     * @param text Text to animate
     * @param iterations Number of animation iterations
     */
    virtual void showIntroAnimation(const String& text, int iterations) = 0;

    /**
     * @brief Set display to normal mode after intro
     */
    virtual void setAfterIntro() = 0;

    /**
     * @brief Check if display is ready
     * @return true if display is ready for operations
     */
    virtual bool isReady() const = 0;

    /**
     * @brief Get display width in pixels
     * @return Display width
     */
    virtual uint16_t getWidth() const = 0;

    /**
     * @brief Get display height in pixels
     * @return Display height
     */
    virtual uint16_t getHeight() const = 0;

    /**
     * @brief Show/display the current content
     */
    virtual void show() = 0;
};
