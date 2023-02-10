#pragma once

#include "Adafruit_NeoTrellis.h"
#include "Structs.h"
#include "ControlCommand.h"
#include "constants.h"

class RTPMainUnit;
class RTPNeoTrellis{
    static Adafruit_NeoTrellis myTrellis;
    static RTPMainUnit* mainUnit;
 public:
    RTPNeoTrellis();
    void begin(RTPMainUnit* _mainUnit);
    void introAnimation();
    static TrellisCallback blink(keyEvent evt);
    void read();
    void writeSequenceStates(RTPSequenceNoteStates seqStates, int color, boolean show=true);
    void writeSceneStates(RTPSequencesState sceneStates);
    void writeBuitCCStates(RTPSequencesState buitCCStates, int color);
    void writeSequenceSettingsPage(SequenceSettings sequenceSettings);
    void writeTransportPage(int color);
    void moveCursor(int cursorPos);
    int colorScaler(int color, int scalar, int max);
};