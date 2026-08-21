#include "BuitPersistenceManager.hpp"
#include "Midi/MidiRouter.hpp"

BuitPersistenceManager::BuitPersistenceManager() {}

BuitPersistenceManager::~BuitPersistenceManager() {}

bool BuitPersistenceManager::fileExists(const String& fileName) {
    return fileExistsOnSD(fileName);
}

String BuitPersistenceManager::sequenceToJson(const RTPEventNoteSequence* sequence) {
    String noteSeqString;
    JsonDocument doc;
    
    doc["t"] = sequence->getType();
    doc["c"] = sequence->getMidiChannel();
    doc["p"] = sequence->getPort();
    
    JsonArray seq = doc["s"].to<JsonArray>();
    for (const RTPEventNotePlus& eventNote : sequence->getEventNoteSequence()) {
        JsonObject note = seq.add<JsonObject>();
        note["r"] = sequence->getType() == DRUM_PART ? eventNote.getEventNote() : eventNote.getEventRead();
        note["v"] = eventNote.eventState() ? eventNote.getEventVelocity() : 0;
        note["l"] = eventNote.getLength();
    }
    serializeJson(doc, noteSeqString);
    return noteSeqString;
}

String BuitPersistenceManager::sceneToJson(const RTPScene* scene) {
    String sceneString;
    JsonDocument doc;
    
    JsonArray sequencesArray = doc["q"].to<JsonArray>();

    int numSequences = scene->getSize();
    for (int i = 0; i < numSequences; i++) {
        const RTPEventNoteSequence* sequence = scene->getSequence(i);
        if (sequence) {
            JsonObject seqObj = sequencesArray.add<JsonObject>();
            seqObj["t"] = sequence->getType();
            seqObj["c"] = sequence->getMidiChannel();
            seqObj["p"] = sequence->getPort();
            JsonArray seqArray = seqObj["s"].to<JsonArray>();
            for (const RTPEventNotePlus& eventNote : sequence->getEventNoteSequence()) {
                JsonObject noteObj = seqArray.add<JsonObject>();
                noteObj["r"] = sequence->getType() == DRUM_PART ? eventNote.getEventNote() : eventNote.getEventRead();
                noteObj["v"] = eventNote.eventState() ? eventNote.getEventVelocity() : 0;
                noteObj["l"] = eventNote.getLength();
            }
        }
    }
    serializeJsonPretty(doc, sceneString);
    return sceneString;
}

String BuitPersistenceManager::sequencerToJson(const RTPSequencer& sequencer) {
    String sequencerString;
    JsonDocument doc;
    
    JsonArray scenesArray = doc["sc"].to<JsonArray>();
    
    int numScenes = sequencer.getNumScenes();
    Serial.print("Number of scenes to serialize: ");
    Serial.println(numScenes);
    
    for (int i = 0; i < numScenes; i++) {
        const RTPScene* scene = sequencer.getScene(i);
        if (scene) {
            JsonObject sceneObj = scenesArray.add<JsonObject>();
            JsonArray sequencesArray = sceneObj["q"].to<JsonArray>();
            int numSequences = scene->getSize();
            Serial.print("Number of sequences in scene: ");
            Serial.println(numSequences);
            for (int j = 0; j < numSequences; j++) {
                const RTPEventNoteSequence* sequence = scene->getSequence(j);
                if (sequence) {
                    JsonObject seqObj = sequencesArray.add<JsonObject>();
                    seqObj["t"] = sequence->getType();
                    seqObj["c"] = sequence->getMidiChannel();
                    seqObj["p"] = sequence->getPort();
                    JsonArray seqArray = seqObj["s"].to<JsonArray>();
                    for (const RTPEventNotePlus& eventNote : sequence->getEventNoteSequence()) {
                        JsonObject noteObj = seqArray.add<JsonObject>();
                        noteObj["r"] = sequence->getType() == DRUM_PART ? eventNote.getEventNote() : eventNote.getEventRead();
                        noteObj["v"] = eventNote.eventState() ? eventNote.getEventVelocity() : 0;
                        noteObj["l"] = eventNote.getLength();
                    }
                }
            }
        }
    }
    size_t len = serializeJson(doc, sequencerString);
    Serial.print("JSON size: ");
    Serial.println(len);
    return sequencerString;
}

bool BuitPersistenceManager::saveSequenceToFile(const RTPEventNoteSequence* sequence, const String& fileName) {
    String jsonData = sequenceToJson(sequence);
    return writeToFile(fileName, jsonData);
}

bool BuitPersistenceManager::saveSceneToFile(const RTPScene* scene, const String& fileName) {
    String jsonData = sceneToJson(scene);
    return writeToFile(fileName, jsonData);
}

bool BuitPersistenceManager::saveSequencerToFile(const RTPSequencer& sequencer, const String& fileName) {
    String jsonData = sequencerToJson(sequencer);
    Serial.println("Saving sequences to file: " + fileName);
    return writeToFile(fileName, jsonData);
}

bool BuitPersistenceManager::loadSequenceFromJson(RTPEventNoteSequence* sequence, const JsonObject& seqObj) {
    int type = seqObj["t"];
    int midiChannel = seqObj["c"];
    int port = seqObj["p"] | 0;  // Default to 0 (routing table) if missing
    sequence->setType(type);
    sequence->setMidiChannel(midiChannel);
    sequence->setPort(port);
    sequence->clearSequence();

    JsonArray notesArray = seqObj["s"];
    for (JsonObject noteObj : notesArray) {
        int read = noteObj["r"];
        int velocity = noteObj["v"];
        int length = noteObj["l"];
        bool isActive = velocity > 0;
        RTPEventNotePlus eventNote(midiChannel, false, read, 0);
        eventNote.setEventRead(read);
        eventNote.setEventState(isActive);
        eventNote.setLength(length);
        if (isActive)
            eventNote.setEventVelocity(velocity);
        sequence->addEventNote(eventNote);
    }
    
    return true;
}

bool BuitPersistenceManager::loadSequencerFromFile(RTPSequencer& sequencer, const String& fileName) {
    String jsonData;
    Serial.println("Loading sequences from file: " + fileName);
    
    if (!readFromFile(fileName, jsonData)) {
        Serial.println("Failed to read sequences file");
        return false;
    }
    
    Serial.println("Loaded JSON data, now parsing");
    return parseAndLoadSequences(sequencer, jsonData);
}

bool BuitPersistenceManager::parseAndLoadSequences(RTPSequencer& sequencer, const String& jsonData) {
    sequencer.stopAndCleanSequencer();
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonData);
    
    if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return false;
    }
    
    JsonArray scenesArray = doc["sc"].as<JsonArray>();
    int numScenes = sequencer.getNumScenes();
    int scenesToProcess = min(numScenes, (int)scenesArray.size());
    
    for (int sceneIdx = 0; sceneIdx < scenesToProcess; sceneIdx++) {
        JsonObject sceneObj = scenesArray[sceneIdx];
        JsonArray sequencesArray = sceneObj["q"].as<JsonArray>();
        
        // Get a handle to the current scene
        RTPScene* scene = sequencer.getScene(sceneIdx);
        if (!scene) continue;
        
        // Load sequences for this scene
        int seqCount = min(scene->getSize(), (int)sequencesArray.size());
        for (int seqIdx = 0; seqIdx < seqCount; seqIdx++) {
            JsonObject seqObj = sequencesArray[seqIdx];
            
            // Get reference to the sequence
            RTPEventNoteSequence* sequence = scene->getSequence(static_cast<uint8_t>(seqIdx));
            if (!sequence) continue;
            
            // Load the sequence data
            loadSequenceFromJson(sequence, seqObj);
        }
    }
    
    Serial.println("Successfully loaded sequences");
    return true;
}

bool BuitPersistenceManager::saveRoutingConfig(const MidiRouter& router, const String& fileName) {
    JsonDocument doc;
    doc["clockOut"] = static_cast<uint8_t>(router.getClockOutputPorts());
    doc["clockIn"]  = static_cast<uint8_t>(router.getClockInputSource());
    
    String jsonData;
    serializeJson(doc, jsonData);
    Serial.println("Saving routing config: " + jsonData);
    return writeToFile(fileName, jsonData);
}

bool BuitPersistenceManager::loadRoutingConfig(MidiRouter& router, const String& fileName) {
    String jsonData;
    if (!readFromFile(fileName, jsonData)) {
        Serial.println("No routing config found, using defaults");
        return false;
    }
    
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonData);
    if (error) {
        Serial.print("Routing config parse error: ");
        Serial.println(error.c_str());
        return false;
    }
    
    if (doc.containsKey("clockOut")) {
        router.setClockOutputPorts(static_cast<MidiPort>(doc["clockOut"].as<uint8_t>()));
    }
    if (doc.containsKey("clockIn")) {
        router.setClockInputSource(static_cast<MidiPort>(doc["clockIn"].as<uint8_t>()));
    }
    
    Serial.println("Routing config loaded");
    return true;
}