#pragma once

#include <cstdint>
#include <stddef.h>
#include <Arduino.h>
#include <WString.h>
#include "Structs.h"

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
     * @brief Display four lines with playback/recording state indicator
     * @param line1 First line of text
     * @param line2 Second line of text
     * @param line3 Third line of text
     * @param line4 Fourth line of text
     * @param state Playback/recording state (play triangle, stop square, rec circle)
     * @param blinkState Blink phase for waiting indicator
     */
    virtual void printFourLinesWithState(const String& line1, const String& line2, const String& line3, const String& line4, SequenceDisplayState state, bool blinkState) = 0;

    /**
     * @brief Show intro animation with scrolling text
     * @param text Text to animate
     * @param iterations Number of animation iterations
     */
    virtual void showIntroAnimation(const String& text, int iterations) = 0;

    /**
     * @brief Show a single frame of the intro scroll animation
     * @param offset Horizontal scroll offset in pixels; start at getWidth(), mutated per frame
     * @param text Text to scroll
     *
     * Allows interleaving display frames with other hardware animations
     * (e.g. button matrix intro frames) in a shared loop.
     */
    virtual void showIntroFrame(int& offset, const String& text) = 0;

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

    /**
     * @brief Update the transversal status-strip model (transport, BPM, scene, seq, ch)
     * @param hud Snapshot of cross-cutting status
     *
     * Default no-op so displays without a status strip needn't implement it.
     */
    virtual void setHud(const HudModel& hud) { (void)hud; }

    /**
     * @brief Set the on-screen legend of trellis-pad functions for this screen
     * @param hints   array of PadHint (label per pad)
     * @param count   number of hints (<= MAX_PAD_HINTS)
     * @param focusedPad  pad to highlight (the one being edited), or -1/255 for none
     *
     * Default no-op so displays without a legend needn't implement it.
     * Call after a print* method — a new content frame clears the legend.
     */
    virtual void setPadLegend(const PadHint* hints, uint8_t count, int8_t focusedPad = -1) {
        (void)hints; (void)count; (void)focusedPad;
    }

    /**
     * @brief Set the on-screen 4x4 cell grid (siblings/context view)
     * @param fillMask  bit i = cell i filled (enabled / has content)
     * @param count     how many cells exist (<=16); the rest are not drawn
     * @param highlight index of the current cell to ring, or -1 for none
     *
     * Default no-op so displays without a grid needn't implement it.
     * Call after a print* method — a new content frame clears the grid.
     */
    virtual void setGrid(uint16_t fillMask, uint8_t count, int8_t highlight) {
        (void)fillMask; (void)count; (void)highlight;
    }
};
