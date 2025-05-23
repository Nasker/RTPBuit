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

struct ThreeReadings {
  int sensorReading1;
  int sensorReading2;
  int sensorReading3;
};

struct SequenceSettings{
  int midiChannel;
  int type;
  int color; 
  int lenght;
};