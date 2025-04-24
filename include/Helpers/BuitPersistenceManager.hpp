#pragma once

#include "RTPEventNoteSequence.h"
#include "RTPSequencer.h"
#include "RTPScene.h"
#include <ArduinoJson.h>
#include "RTPSDManager.hpp"

class BuitPersistenceManager {
public:
    BuitPersistenceManager();
    ~BuitPersistenceManager();
    
    // Serialization methods
    String sequenceToJson(const RTPEventNoteSequence* sequence);
    String sceneToJson(const RTPScene* scene);
    String sequencerToJson(const RTPSequencer& sequencer);
    
    // Persistence methods
    bool saveSequencerToFile(const RTPSequencer& sequencer, const String& fileName = "sequences.json");
    bool saveSceneToFile(const RTPScene* scene, const String& fileName);
    bool saveSequenceToFile(const RTPEventNoteSequence* sequence, const String& fileName);
    
    // Loading methods
    bool loadSequencerFromFile(RTPSequencer& sequencer, const String& fileName = "sequences.json");
    bool parseAndLoadSequences(RTPSequencer& sequencer, const String& jsonData);
    bool loadSequenceFromJson(RTPEventNoteSequence* sequence, const JsonObject& seqObj);
};