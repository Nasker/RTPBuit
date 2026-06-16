#pragma once

#include <cstdint>

/**
 * @brief User interface configuration constants
 * 
 * Centralized UI constants to eliminate magic numbers and improve
 * maintainability of user interface elements.
 */
namespace UiConfig {
    // Transport Button Layout
    namespace Transport {
        constexpr uint8_t PLAY_BUTTON = 0;
        constexpr uint8_t STOP_BUTTON = 1;
        constexpr uint8_t REC_BUTTON = 2;
        constexpr uint8_t TAP_BUTTON = 3;
        constexpr uint8_t MODE_BUTTON = 4;
        constexpr uint8_t BUTTON_COUNT = 5;
    }

    // Color Scheme
    namespace Colors {
        // Transport Colors
        constexpr uint32_t PLAY_COLOR = 0x00FF00;      // Green
        constexpr uint32_t STOP_COLOR = 0x0000FF;      // Blue
        constexpr uint32_t REC_COLOR = 0xFF0000;       // Red
        constexpr uint32_t TAP_COLOR = 0xFFFF00;       // Yellow
        constexpr uint32_t MODE_COLOR = 0xFFFFFF;      // White

        // Sequence Type Colors
        constexpr uint32_t DRUM_COLOR = 0xFF8800;      // Orange
        constexpr uint32_t BASS_COLOR = 0x00FFAA;      // Cyan
        constexpr uint32_t MONO_COLOR = 0xFF00FF;      // Magenta
        constexpr uint32_t POLY_COLOR = 0x88FF00;      // Lime
        constexpr uint32_t CONTROL_COLOR = 0x8888FF;   // Light Blue
        constexpr uint32_t HARMONY_COLOR = 0xFFAA00;   // Gold

        // State Colors
        constexpr uint32_t ACTIVE_COLOR = 0xFFFFFF;    // White
        constexpr uint32_t INACTIVE_COLOR = 0x101010;  // Dim
        constexpr uint32_t WAITING_COLOR = 0xFFFF00;   // Yellow (recording waiting)
        constexpr uint32_t RECORDING_COLOR = 0xFF0000; // Red (recording active)

        // Dim Colors
        constexpr uint8_t DIM_BRIGHTNESS = 128;        // 50% brightness
        constexpr uint8_t VERY_DIM_BRIGHTNESS = 64;    // 25% brightness
    }

    // Animation Configuration
    namespace Animation {
        constexpr uint16_t INTRO_ITERATIONS = 55;
        constexpr uint16_t SCROLL_SPEED = 1;
        constexpr uint16_t BLINK_PERIOD_MS = 500;
        constexpr uint8_t BLINK_COUNTER_MAX = 10;
    }

    // Text Display Configuration
    namespace Text {
        constexpr uint8_t MAX_LINE_LENGTH = 32;
        constexpr uint8_t LINE_COUNT = 4;
        constexpr const char* DEFAULT_TEXT = "";
        constexpr const char* INTRO_TEXT = "I'm RTP's BUIT!!";
    }

    // Layout Configuration
    namespace Layout {
        constexpr uint8_t BUTTONS_PER_ROW = 4;
        constexpr uint8_t BUTTONS_PER_COL = 4;
        constexpr uint8_t MODIFIER_ROW = 3;          // Top row (0-indexed)
        constexpr uint8_t MELODIC_ROWS = 3;          // Bottom 3 rows
        constexpr uint8_t FIRST_MELODIC_ROW = 0;      // Bottom row
        constexpr uint8_t LAST_MELODIC_ROW = 2;       // Third row from bottom
    }

    // Parameter Configuration
    namespace Parameters {
        constexpr uint8_t PARAM_COUNT = 4;           // Type, Channel, Color, Length
        constexpr int PARAM_MIN_VALUE = 0;
        constexpr int PARAM_MAX_VALUE = 127;
        constexpr int PARAM_COLOR_MAX = 31;
        constexpr int PARAM_LENGTH_MAX = 255;
    }

    // Pattern Bank UI Configuration
    namespace PatternBank {
        constexpr uint8_t PAGE_HUE_STEPS = 24;        // Hue steps for page colors
        constexpr uint32_t EMPTY_SLOT_COLOR = 0x101010;  // Dim for empty slots
        constexpr uint32_t EXISTS_SLOT_COLOR = 0xFFFFFF; // Bright for existing slots
    }

    // Performance Configuration
    namespace Performance {
        constexpr uint8_t UPDATE_RATE_HZ = 100;       // 10ms update period
        constexpr uint8_t DISPLAY_REFRESH_RATE = 30;  // Display refresh rate
        constexpr uint16_t DEBOUNCE_TIME_MS = 50;     // Button debounce
    }
}
