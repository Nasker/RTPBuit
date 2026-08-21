#pragma once


struct RTPSequenceNoteStates{
  bool val[16];
  int velocity[16];
};

struct RTPSequenceState{
  bool state;
  int color;
};

struct RTPSequencesState{
  RTPSequenceState sequenceState[16];
};

struct SequenceSettings{
  int midiChannel;
  int type;
  int color; 
  int lenght;
  int port;
};

// Display states for sequence recording/playback indicator
enum class SequenceDisplayState {
    Playing,   // Scene playing or sequence enabled - show play triangle
    Stopped,   // Scene stopped - show stop square
    Waiting,   // Armed for recording, waiting for position 0 - blink REC circle
    Recording  // Actively recording - solid REC circle
};