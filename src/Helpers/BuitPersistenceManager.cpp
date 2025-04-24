#include "BuitPersistenceManager.hpp"

BuitPersistenceManager::BuitPersistenceManager() {}

BuitPersistenceManager::~BuitPersistenceManager() {}

// Convert an individual sequence to JSON string
String BuitPersistenceManager::sequenceToJson(const RTPEventNoteSequence* sequence) {
    String noteSeqString;
    StaticJsonDocument<4096> doc;
    
    doc["t"] = sequence->getType();
    doc["c"] = sequence->getMidiChannel();
    
    JsonArray seq = doc.createNestedArray("s");
    for (const RTPEventNotePlus& eventNote : sequence->getEventNoteSequence()) {
        JsonObject note = seq.createNestedObject();
        note["r"] = eventNote.getEventRead();
        note["v"] = eventNote.eventState() ? eventNote.getEventVelocity() : 0;
        note["l"] = eventNote.getLength();
    }
    
    serializeJson(doc, noteSeqString);
    return noteSeqString;
}

// Convert all sequences in a scene to JSON string
String BuitPersistenceManager::sceneToJson(const RTPScene* scene) {
    String sceneString;
    DynamicJsonDocument doc(32768);  // Adjust size based on your scene complexity
    
    JsonArray sequencesArray = doc.createNestedArray("q");
    
    // Get number of sequences in the scene
    int numSequences = scene->getSize();
    
    // Add each sequence as a JSON object
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

// Convert the entire sequencer to JSON string
String BuitPersistenceManager::sequencerToJson(const RTPSequencer& sequencer) {
    String sequencerString;
    DynamicJsonDocument doc(262144);  // Increased to 256KB
    
    JsonArray scenesArray = doc.createNestedArray("sc");
    
    // Get number of scenes in the sequencer
    int numScenes = sequencer.getNumScenes();
    Serial.print("Number of scenes to serialize: ");
    Serial.println(numScenes);
    
    // Add each scene as a JSON object
    for (int i = 0; i < numScenes; i++) {
        const RTPScene* scene = sequencer.getScene(i);
        if (scene) {
            Serial.print("Processing scene ");
            Serial.println(i);
            
            // Create a scene object directly
            JsonObject sceneObj = scenesArray.createNestedObject();
            
            // Create sequences array
            JsonArray sequencesArray = sceneObj.createNestedArray("q");
            
            // Get number of sequences in the scene
            int numSequences = scene->getSize();
            Serial.print("Number of sequences in scene: ");
            Serial.println(numSequences);
            
            // Add each sequence to the scene
            for (int j = 0; j < numSequences; j++) {
                const RTPEventNoteSequence* sequence = scene->getSequence(j);
                if (sequence) {
                    // Create a sequence object
                    JsonObject seqObj = sequencesArray.createNestedObject();
                    
                    // Add sequence properties
                    seqObj["t"] = sequence->getType();
                    seqObj["c"] = sequence->getMidiChannel();
                    
                    // Create sequence array
                    JsonArray seqArray = seqObj.createNestedArray("s");
                    
                    // Add each note to the sequence
                    for (const RTPEventNotePlus& eventNote : sequence->getEventNoteSequence()) {
                        JsonObject noteObj = seqArray.createNestedObject();
                        noteObj["r"] = eventNote.getEventRead();
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

// Save an individual sequence to a file
bool BuitPersistenceManager::saveSequenceToFile(const RTPEventNoteSequence* sequence, const String& fileName) {
    String jsonData = sequenceToJson(sequence);
    return writeToFile(fileName, jsonData);
}

// Save a scene to a file
bool BuitPersistenceManager::saveSceneToFile(const RTPScene* scene, const String& fileName) {
    String jsonData = sceneToJson(scene);
    return writeToFile(fileName, jsonData);
}

// Save the entire sequencer to a file
bool BuitPersistenceManager::saveSequencerToFile(const RTPSequencer& sequencer, const String& fileName) {
    String jsonData = sequencerToJson(sequencer);
    Serial.println("Saving sequences to file: " + fileName);
    return writeToFile(fileName, jsonData);
}

// Load a sequence from JSON object
bool BuitPersistenceManager::loadSequenceFromJson(RTPEventNoteSequence* sequence, const JsonObject& seqObj) {
    // Extract basic properties
    int type = seqObj["t"];
    int midiChannel = seqObj["c"];
    
    // Update sequence properties
    sequence->setType(type);
    sequence->setMidiChannel(midiChannel);
    sequence->clearSequence();
    
    // Load all the notes
    JsonArray notesArray = seqObj["s"];
    for (JsonObject noteObj : notesArray) {
        bool read = noteObj["r"];
        int velocity = noteObj["v"];
        int length = noteObj["l"];
        
        // Create a new event note with the loaded data
        RTPEventNotePlus eventNote(midiChannel, read, velocity, length);
        eventNote.setEventState(velocity > 0);
        sequence->addEventNote(eventNote);
    }
    
    return true;
}

// Load sequences from file into sequencer
bool BuitPersistenceManager::loadSequencesFromFile(RTPSequencer& sequencer, const String& fileName) {
    String jsonData;
    Serial.println("Loading sequences from file: " + fileName);
    
    if (!readFromFile(fileName, jsonData)) {
        Serial.println("Failed to read sequences file");
        return false;
    }
    
    Serial.println("Loaded JSON data, now parsing");
    return parseAndLoadSequences(sequencer, jsonData);
}

// Parse JSON data and populate the sequencer
bool BuitPersistenceManager::parseAndLoadSequences(RTPSequencer& sequencer, const String& jsonData) {
    // Stop any ongoing playback and reset sequences
    sequencer.stopAndCleanSequencer();
    
    // Create a JSON document large enough for your sequences
    DynamicJsonDocument doc(262144); // Adjust size as needed
    DeserializationError error = deserializeJson(doc, jsonData);
    
    if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return false;
    }
    
    // Process each scene in the sequencer
    JsonArray scenesArray = doc["sc"].as<JsonArray>();
    int numScenes = sequencer.getNumScenes();
    
    // Make sure we don't exceed the array bounds
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