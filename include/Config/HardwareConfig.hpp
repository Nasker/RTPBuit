#pragma once

#include <cstdint>

/**
 * @brief Hardware configuration constants
 * 
 * Centralized hardware pin definitions and constants to eliminate magic numbers
 * and improve maintainability.
 */
namespace HardwareConfig {
    // Pin Definitions
    namespace Pins {
        constexpr uint8_t TRELLIS_INT = 9;
        constexpr uint8_t BUTTON = 10;
        constexpr uint8_t ROT_LEFT = 11;
        constexpr uint8_t ROT_RIGHT = 12;
        constexpr uint8_t SENSOR_1 = 2;
        constexpr uint8_t SENSOR_2 = 3;
        constexpr uint8_t SENSOR_3 = 4;
    }

    // I2C Addresses
    namespace I2C {
        constexpr uint8_t OLED_SCREEN = 0x3C;
        constexpr uint8_t VL53L0X_SENSOR1 = 0x29;
        constexpr uint8_t VL53L0X_SENSOR2 = 0x2A;
        constexpr uint8_t VL53L0X_SENSOR3 = 0x2B;
    }

    // Display Configuration
    namespace Display {
        constexpr uint16_t SCREEN_WIDTH = 128;
        constexpr uint16_t SCREEN_HEIGHT = 64;
        constexpr uint8_t TEXT_SIZE = 1;
    }

    // MIDI Configuration
    namespace Midi {
        constexpr uint32_t BAUD_RATE = 31250;
        constexpr uint8_t CHANNEL_COUNT = 16;
        constexpr uint8_t NOTE_COUNT = 128;
        constexpr uint8_t CC_COUNT = 128;
    }

    // Sensor Configuration
    namespace Sensors {
        constexpr int DEFAULT_MAX_READING = 306;
        constexpr int DEFAULT_MIN_READING = 50;
        constexpr uint8_t SENSOR_COUNT = 3;
    }

    // Button Matrix Configuration
    namespace ButtonMatrix {
        constexpr uint8_t GRID_SIZE = 16;  // 4x4 grid
        constexpr uint8_t GRID_ROWS = 4;
        constexpr uint8_t GRID_COLS = 4;
        constexpr uint8_t TRANSPORT_BUTTONS = 5;  // Play, Stop, Rec, Tap, Mode
        constexpr uint8_t MODIFIER_BUTTONS = 4;   // Top row for chord modifiers
        constexpr uint8_t MELODIC_BUTTONS = 12;   // Bottom 3 rows for notes
    }

    // Timing Configuration
    namespace Timing {
        constexpr uint32_t UPDATE_PERIOD_MS = 10;    // Sensor update period
        constexpr uint32_t DEBOUNCE_MS = 50;         // Button debounce
        constexpr uint32_t LONG_PRESS_MS = 1000;     // Long press threshold
    }
}
