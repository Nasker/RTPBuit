#pragma once

#include <cstdint>

struct RTPSequenceNoteStates{
  bool val[16];
  int velocity[16];
};

struct RTPSequenceState{
  bool state = false;   // enabled/playing
  int color  = 0;
};

struct RTPSequencesState{
  RTPSequenceState sequenceState[16];
};

struct SequenceSettings{
  int midiChannel;
  int type;
  int color; 
  int lenght;
  int input;
  int port;
  int clockDivider;   // grid / step-rate index (0-10)
};

// On-screen legend of what each trellis pad does in the current state.
// Rendered as a mini grid echoing the physical pad layout so it maps to
// muscle memory. Labels are short string literals (static storage).
#define MAX_PAD_HINTS 8
struct PadHint {
    uint8_t     pad;     // logical trellis pad 0-15 (col = pad%4, row = pad/4)
    const char* label;   // short caption, e.g. "PLAY", "CH", "DIV"
};

// Display states for sequence recording/playback indicator
enum class SequenceDisplayState {
    Playing,   // Scene playing or sequence enabled - show play triangle
    Stopped,   // Scene stopped - show stop square
    Waiting,   // Armed for recording, waiting for position 0 - blink REC circle
    Recording  // Actively recording - solid REC circle
};

// Transversal status shown in the persistent top strip on every screen.
// Primitive fields only (no SyncMode dep) so Structs.h stays lightweight.
struct HudModel {
    SequenceDisplayState transport = SequenceDisplayState::Stopped;
    bool    syncInternal = false;  // true = internal clock (show BPM), false = external (show EXT)
    float   bpm          = 0;
    uint8_t beat         = 0;      // beat-in-bar 0-3 (from the clock pulse counter)
    uint8_t scene        = 0;      // 0-based current scene
    uint8_t activeSeq    = 0;      // enabled/playing sequences in the scene
    bool    blink        = false;  // blink phase for the Waiting glyph

    bool operator==(const HudModel& o) const {
        return transport == o.transport && syncInternal == o.syncInternal &&
               bpm == o.bpm && beat == o.beat && scene == o.scene &&
               activeSeq == o.activeSeq && blink == o.blink;
    }
    bool operator!=(const HudModel& o) const { return !(*this == o); }
};