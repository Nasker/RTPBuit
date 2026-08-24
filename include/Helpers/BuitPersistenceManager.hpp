#pragma once

#include "RTPEventNoteSequence.h"
#include "RTPSequencer.h"
#include "RTPScene.h"
#include <ArduinoJson.h>
#include "RTPSDManager.hpp"

class MidiRouter;

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
    bool parseAndLoadSequences(RTPSequencer& sequencer, File& file);
    bool loadSequenceFromJson(RTPEventNoteSequence* sequence, const JsonObject& seqObj);
    
    // Routing config persistence
    bool saveRoutingConfig(const MidiRouter& router, const String& fileName = "routing.json");
    bool loadRoutingConfig(MidiRouter& router, const String& fileName = "routing.json");
    
    // Query methods
    bool fileExists(const String& fileName);

private:
    bool parseAndLoadFromDoc(RTPSequencer& sequencer, JsonDocument& doc);
    bool saveSequencerToBinary(const RTPSequencer& sequencer, const String& fileName);
    bool loadSequencerFromBinary(RTPSequencer& sequencer, File& file);
    bool saveSequenceToBinary(RTPEventNoteSequence* sequence, File& file);
    bool loadSequenceFromBinary(RTPEventNoteSequence* sequence, File& file);
};