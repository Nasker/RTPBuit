#pragma once

#define TRANSPORT_COLOR     0xAAAAAA
#define CURSOR_COLOR        0xFFFFFF

#define DRUM_COLOR          0xFF0000
#define BASS_SYNTH_COLOR    0x0000FF
#define MONO_SYNTH_COLOR    0x00FF00
#define POLY_SYNTH_COLOR    0xFFFF00
#define CONTROL_TRACK_COLOR 0x00FFFF
#define HARMONY_TRACK_COLOR 0xFF00FF

// Color wheel indices (0-31) matching colorWheel[] in ColorFunctions.h
// 0=red, 8=green, 16=cyan, 21=blue, 24=purple, 27=magenta
#define DRUM_COLOR_IDX          0   // red
#define BASS_SYNTH_COLOR_IDX    21  // blue
#define MONO_SYNTH_COLOR_IDX    8   // green
#define POLY_SYNTH_COLOR_IDX    5   // yellow-green
#define CONTROL_TRACK_COLOR_IDX 16  // cyan
#define HARMONY_TRACK_COLOR_IDX 27  // magenta
