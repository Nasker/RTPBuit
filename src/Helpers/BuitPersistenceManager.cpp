#include "BuitPersistenceManager.hpp"

BuitPersistenceManager::BuitPersistenceManager() {}

BuitPersistenceManager::~BuitPersistenceManager() {}

String BuitPersistenceManager::sequenceToJson(const RTPEventNoteSequence* sequence) {
    String noteSeqString;
    StaticJsonDocument<4096> doc;
    
    doc["t"] = sequence->getType();
    doc["c"] = sequence->getMidiChannel();
    
    JsonArray seq = doc.createNestedArray("s");
    for (const RTPEventNotePlus& eventNote : sequence->getEventNoteSequence()) {
        JsonObject note = seq.createNestedObject();
        note["r"] = sequence->getType() == DRUM_PART ? eventNote.getEventNote() : eventNote.getEventRead();
        note["v"] = eventNote.eventState() ? eventNote.getEventVelocity() : 0;
        note["l"] = eventNote.getLength();
    }
    serializeJson(doc, noteSeqString);
    return noteSeqString;
}

String BuitPersistenceManager::sceneToJson(const RTPScene* scene) {
    String sceneString;
    DynamicJsonDocument doc(32768);  // Adjust size based on your scene complexity
    
    JsonArray sequencesArray = doc.createNestedArray("q");

    int numSequences = scene->getSize();
    for (int i = 0; i < numSequences; i++) {
        const RTPEventNoteSequence* sequence = scene->getSequence(i);
        if (sequence) {
            String seqJson = sequenceToJson(sequence);
            JsonObject seqObj = sequencesArray.createNestedObject();
            
            // Parse the sequence JSON and add it to the scene object
            StaticJsonDocument<4096> seqDoc;
            deserializeJson(seqDoc, seqJson);
            seqObj.set(seqDoc.as<JsonObject>());
        }
    }
    serializeJsonPretty(doc, sceneString);
    return sceneString;
}

String BuitPersistenceManager::sequencerToJson(const RTPSequencer& sequencer) {
    String sequencerString;
    DynamicJsonDocument doc(262144);  // Increased to 256KB
    
    JsonArray scenesArray = doc.createNestedArray("sc");
    
    int numScenes = sequencer.getNumScenes();
    Serial.print("Number of scenes to serialize: ");
    Serial.println(numScenes);
    
    for (int i = 0; i < numScenes; i++) {
        const RTPScene* scene = sequencer.getScene(i);
        if (scene) {
            JsonObject sceneObj = scenesArray.createNestedObject();
            JsonArray sequencesArray = sceneObj.createNestedArray("q");
            int numSequences = scene->getSize();
            Serial.print("Number of sequences in scene: ");
            Serial.println(numSequences);
            for (int j = 0; j < numSequences; j++) {
                const RTPEventNoteSequence* sequence = scene->getSequence(j);
                if (sequence) {
                    JsonObject seqObj = sequencesArray.createNestedObject();
                    seqObj["t"] = sequence->getType();
                    seqObj["c"] = sequence->getMidiChannel();
                    JsonArray seqArray = seqObj.createNestedArray("s");
                    for (const RTPEventNotePlus& eventNote : sequence->getEventNoteSequence()) {
                        JsonObject noteObj = seqArray.createNestedObject();
                        noteObj["r"] = sequence->getType() == DRUM_PART ? eventNote.getEventNote() : eventNote.getEventRead();
                        noteObj["v"] = eventNote.eventState() ? eventNote.getEventVelocity() : 0;
                        noteObj["l"] = eventNote.getLength();
                    }
                }
            }
        }
    }
    doc.shrinkToFit();
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
    sequence->setType(type);
    sequence->setMidiChannel(midiChannel);
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
            RTPEventNoteSequence* sequence = scene->getSequence(seqIdx);
            if (!sequence) continue;
            
            // Load the sequence data
            loadSequenceFromJson(sequence, seqObj);
        }
    }
    
    Serial.println("Successfully loaded sequences");
    return true;
}