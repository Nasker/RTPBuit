#include "BuitPersistenceManager.hpp"

BuitPersistenceManager::BuitPersistenceManager() {}

BuitPersistenceManager::~BuitPersistenceManager() {}

// Convert an individual sequence to JSON string
String BuitPersistenceManager::sequenceToJson(const RTPEventNoteSequence* sequence) {
    String noteSeqString;
    StaticJsonDocument<1536> doc;
    
    doc["type"] = sequence->getType();
    doc["ch"] = sequence->getMidiChannel();
    
    JsonArray seq = doc.createNestedArray("seq");
    for (const RTPEventNotePlus& eventNote : sequence->getEventNoteSequence()) {
        JsonObject note = seq.createNestedObject();
        note["read"] = eventNote.getEventRead();
        note["vel"] = eventNote.eventState() ? eventNote.getEventVelocity() : 0;
        note["len"] = eventNote.getLength();
    }
    
    serializeJsonPretty(doc, noteSeqString);
    return noteSeqString;
}

// Convert all sequences in a scene to JSON string
String BuitPersistenceManager::sceneToJson(const RTPScene* scene) {
    String sceneString;
    DynamicJsonDocument doc(8192);  // Adjust size based on your scene complexity
    
    JsonArray sequencesArray = doc.createNestedArray("sequences");
    
    // Get number of sequences in the scene
    int numSequences = scene->getSize();
    
    // Add each sequence as a JSON object
    for (int i = 0; i < numSequences; i++) {
        const RTPEventNoteSequence* sequence = scene->getSequence(i);
        if (sequence) {
            String seqJson = sequenceToJson(sequence);
            JsonObject seqObj = sequencesArray.createNestedObject();
            
            // Parse the sequence JSON and add it to the scene object
            StaticJsonDocument<1536> seqDoc;
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
    DynamicJsonDocument doc(32768);  // Large document for all scenes
    
    JsonArray scenesArray = doc.createNestedArray("scenes");
    
    // Get number of scenes in the sequencer
    int numScenes = sequencer.getNumScenes();
    
    // Add each scene as a JSON object
    for (int i = 0; i < numScenes; i++) {
        const RTPScene* scene = sequencer.getScene(i);
        if (scene) {
            String sceneJson = sceneToJson(scene);
            
            // Parse the scene JSON and add it to the sequencer object
            DynamicJsonDocument sceneDoc(8192);
            deserializeJson(sceneDoc, sceneJson);
            JsonObject sceneObj = scenesArray.createNestedObject();
            sceneObj.set(sceneDoc.as<JsonObject>());
        }
    }
    
    serializeJsonPretty(doc, sequencerString);
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
    int type = seqObj["type"];
    int midiChannel = seqObj["ch"];
    
    // Update sequence properties
    sequence->setType(type);
    sequence->setMidiChannel(midiChannel);
    sequence->clearSequence();
    
    // Load all the notes
    JsonArray notesArray = seqObj["seq"];
    for (JsonObject noteObj : notesArray) {
        bool read = noteObj["read"];
        int velocity = noteObj["vel"];
        int length = noteObj["len"];
        
        // Create a new event note with the loaded data
        RTPEventNotePlus eventNote;
        eventNote.setEventRead(read);
        eventNote.setEventVelocity(velocity);
        eventNote.setLength(length);
        eventNote.setEventState(velocity > 0);
        
        // Add it to the sequence
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
    DynamicJsonDocument doc(32768); // Adjust size as needed
    DeserializationError error = deserializeJson(doc, jsonData);
    
    if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return false;
    }
    
    // Process each scene in the sequencer
    JsonArray scenesArray = doc["scenes"].as<JsonArray>();
    int numScenes = sequencer.getNumScenes();
    
    // Make sure we don't exceed the array bounds
    int scenesToProcess = min(numScenes, (int)scenesArray.size());
    
    for (int sceneIdx = 0; sceneIdx < scenesToProcess; sceneIdx++) {
        JsonObject sceneObj = scenesArray[sceneIdx];
        JsonArray sequencesArray = sceneObj["sequences"].as<JsonArray>();
        
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