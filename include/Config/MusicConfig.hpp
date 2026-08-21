#pragma once

#include <cstdint>

// Undefine macros that conflict with constants
#undef BASE_NOTE
#undef BASS_BASE_NOTE
#undef N_SCENES
#undef N_PAGES
#undef N_TYPES
#undef N_COLORS
#undef SEQ_BLOCK_SIZE
#undef SCENE_BLOCK_SIZE
#undef BASS_OCTAVES
#undef SYNTH_OCTAVES
#undef POLY_OCTAVES

/**
 * @brief Music theory and performance configuration
 * 
 * Centralized musical constants and configuration to eliminate magic numbers
 * and improve maintainability of musical features.
 */
namespace MusicConfig {
    // Note Configuration
    namespace Notes {
        constexpr uint8_t NOTES_PER_OCTAVE = 12;
        constexpr uint8_t BASE_NOTE = 36;        // C2
        constexpr uint8_t BASS_BASE_NOTE = 12;   // C0
        constexpr uint8_t MAX_MIDI_NOTE = 127;
        constexpr uint8_t MIN_MIDI_NOTE = 0;
    }

    // Octave Configuration
    namespace Octaves {
        constexpr uint8_t BASS_OCTAVES = 3;
        constexpr uint8_t SYNTH_OCTAVES = 5;
        constexpr uint8_t POLY_OCTAVES = 4;
        constexpr uint8_t SEMITONES_PER_OCTAVE = 12;
    }

    // Velocity Configuration
    namespace Velocity {
        constexpr uint8_t MAX_VELOCITY = 127;
        constexpr uint8_t MIN_VELOCITY = 0;
        constexpr uint8_t DEFAULT_VELOCITY = 90;
        constexpr uint8_t LIVE_VELOCITY = 90;
    }

    // Chord Configuration
    namespace Chords {
        constexpr uint8_t MAX_CHORD_STEPS = 7;
        constexpr uint8_t MAX_CHORD_TYPES = 16;
        constexpr uint8_t N_CHORDION_KEYS = 4;
        constexpr uint8_t MAX_ACTIVE_CHORDS = 16;
    }

    // Sequence Configuration
    namespace Sequences {
        constexpr uint8_t N_SCENES = 3;
        constexpr uint8_t N_PAGES = 16;
        constexpr uint8_t N_TYPES = 6;
        constexpr uint8_t N_COLORS = 32;
        constexpr uint16_t SEQ_BLOCK_SIZE = 16;
        constexpr uint16_t SCENE_BLOCK_SIZE = 16;
        constexpr uint16_t MAX_SEQUENCE_LENGTH = 256;
    }

    // Pattern Bank Configuration
    namespace PatternBank {
        constexpr uint8_t PAGES = 16;
        constexpr uint8_t SLOTS = 16;
        constexpr uint16_t TOTAL_SLOTS = PAGES * SLOTS;
    }

    // Timing Configuration
    namespace Timing {
        constexpr uint8_t CLOCK_GRID = 6;         // Step every 6 ticks = 16th note (24 PPQN / 4)
        constexpr uint8_t FINE_GRID = 3;          // 32nd-note resolution (24 PPQN / 8)
        constexpr uint16_t TICKS_PER_BAR = 96;    // 24 PPQN * 4 beats
        constexpr uint8_t MIN_BPM = 40;
        constexpr uint8_t MAX_BPM = 240;
        constexpr float DEFAULT_BPM = 120.0f;
    }

    // Three-Axis Configuration
    namespace ThreeAxis {
        constexpr uint8_t SENSOR_READING_RANGE = 128;  // 0-127
        constexpr uint8_t HYSTERESIS_MARGIN = 3;       // 1/3 of slot width approximation
        constexpr uint8_t LEFT_AXIS = 0;
        constexpr uint8_t CENTER_AXIS = 1;
        constexpr uint8_t RIGHT_AXIS = 2;
    }

    // Control Change Configuration
    namespace ControlChange {
        constexpr uint8_t HARMMY_CHANNEL = 1;
        constexpr uint8_t HARMMY_CONTROL_START = 0;
        constexpr uint8_t HARMMY_CONTROL_END = 15;
        constexpr uint8_t HARMMY_VALUE_MAX = 15;
    }

    // Color Configuration
    namespace Colors {
        constexpr uint32_t COLOR_OFF = 0x000000;
        constexpr uint32_t COLOR_WHITE = 0xFFFFFF;
        constexpr uint32_t COLOR_RED = 0xFF0000;
        constexpr uint32_t COLOR_GREEN = 0x00FF00;
        constexpr uint32_t COLOR_BLUE = 0x0000FF;
        constexpr uint32_t COLOR_YELLOW = 0xFFFF00;
        constexpr uint32_t COLOR_DIM_FACTOR = 128;  // For dimmed colors
    }

    // Recording Configuration
    namespace Recording {
        constexpr uint8_t DEFAULT_QUANTIZE_STRENGTH = 50;
        constexpr uint8_t MIN_QUANTIZE_STRENGTH = 0;
        constexpr uint8_t MAX_QUANTIZE_STRENGTH = 100;
        constexpr uint8_t DEFAULT_SWING = 0;
        constexpr uint8_t MIN_SWING = 0;
        constexpr uint8_t MAX_SWING = 100;
    }
}
