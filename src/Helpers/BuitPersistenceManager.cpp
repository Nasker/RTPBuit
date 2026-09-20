#include "BuitPersistenceManager.hpp"
#include "Midi/MidiRouter.hpp"
#include <cstring>

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
    doc["i"] = sequence->getInput();
    doc["l"] = sequence->getLength();
    doc["d"] = sequence->getClockDivider();
    doc["e"] = sequence->isCurrentSequenceEnabled() ? 1 : 0;
    doc["n"] = sequence->getName();
    doc["u"] = sequence->getUsbHostLabel();
    
    JsonArray seq = doc["s"].to<JsonArray>();
    for (const RTPEventNotePlus& eventNote : sequence->getEventNoteSequence()) {
        JsonObject note = seq.add<JsonObject>();
        note["r"] = sequence->getType() == DRUM_PART ? eventNote.getEventNote() : eventNote.getEventRead();
        note["v"] = eventNote.eventState() ? eventNote.getEventVelocity() : 0;
        note["l"] = eventNote.getLength();
        note["o"] = eventNote.getMicroOffset();
    }
    serializeJson(doc, noteSeqString);
    return noteSeqString;
}

String BuitPersistenceManager::sceneToJson(const RTPScene* scene) {
    String sceneString;
    JsonDocument doc;

    doc["n"] = scene->getName();
    doc["sel"] = scene->getSelectedSequenceIndex();
    JsonArray sequencesArray = doc["q"].to<JsonArray>();

    int numSequences = scene->getSize();
    for (int i = 0; i < numSequences; i++) {
        const RTPEventNoteSequence* sequence = scene->getSequence(i);
        if (sequence) {
            JsonObject seqObj = sequencesArray.add<JsonObject>();
            seqObj["t"] = sequence->getType();
            seqObj["c"] = sequence->getMidiChannel();
            seqObj["p"] = sequence->getPort();
            seqObj["i"] = sequence->getInput();
            seqObj["l"] = sequence->getLength();
            seqObj["d"] = sequence->getClockDivider();
            seqObj["e"] = sequence->isCurrentSequenceEnabled() ? 1 : 0;
            seqObj["n"] = sequence->getName();
            seqObj["u"] = sequence->getUsbHostLabel();
            JsonArray seqArray = seqObj["s"].to<JsonArray>();
            for (const RTPEventNotePlus& eventNote : sequence->getEventNoteSequence()) {
                JsonObject noteObj = seqArray.add<JsonObject>();
                noteObj["r"] = sequence->getType() == DRUM_PART ? eventNote.getEventNote() : eventNote.getEventRead();
                noteObj["v"] = eventNote.eventState() ? eventNote.getEventVelocity() : 0;
                noteObj["l"] = eventNote.getLength();
                        noteObj["o"] = eventNote.getMicroOffset();
            }
        }
    }
    serializeJsonPretty(doc, sceneString);
    return sceneString;
}

String BuitPersistenceManager::sequencerToJson(const RTPSequencer& sequencer) {
    String sequencerString;
    JsonDocument doc;
    
    doc["sel"] = sequencer.getSelectScene();
    JsonArray scenesArray = doc["sc"].to<JsonArray>();
    
    int numScenes = sequencer.getNumScenes();
    Serial.print("Number of scenes to serialize: ");
    Serial.println(numScenes);
    
    for (int i = 0; i < numScenes; i++) {
        const RTPScene* scene = sequencer.getScene(i);
        if (scene) {
            JsonObject sceneObj = scenesArray.add<JsonObject>();
            sceneObj["n"] = scene->getName();
            sceneObj["sel"] = scene->getSelectedSequenceIndex();
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
                    seqObj["i"] = sequence->getInput();
                    seqObj["l"] = sequence->getLength();
                    seqObj["d"] = sequence->getClockDivider();
                    seqObj["e"] = sequence->isCurrentSequenceEnabled() ? 1 : 0;
                    seqObj["n"] = sequence->getName();
                    seqObj["u"] = sequence->getUsbHostLabel();
                    JsonArray seqArray = seqObj["s"].to<JsonArray>();
                    for (const RTPEventNotePlus& eventNote : sequence->getEventNoteSequence()) {
                        JsonObject noteObj = seqArray.add<JsonObject>();
                        noteObj["r"] = sequence->getType() == DRUM_PART ? eventNote.getEventNote() : eventNote.getEventRead();
                        noteObj["v"] = eventNote.eventState() ? eventNote.getEventVelocity() : 0;
                        noteObj["l"] = eventNote.getLength();
                        noteObj["o"] = eventNote.getMicroOffset();
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
    Serial.println("Saving sequences to file: " + fileName);
    return saveSequencerToBinary(sequencer, fileName);
}

bool BuitPersistenceManager::loadSequenceFromJson(RTPEventNoteSequence* sequence, const JsonObject& seqObj) {
    // Support both current (t/c/p/i/l/s/r/v/l) and legacy (type/ch/seq/read/vel/len) keys
    int type = seqObj["t"].is<int>() ? seqObj["t"].as<int>() : seqObj["type"].as<int>();
    int midiChannel = seqObj["c"].is<int>() ? seqObj["c"].as<int>() : seqObj["ch"].as<int>();
    int port = seqObj["p"].is<int>() ? seqObj["p"].as<int>() : 0;
    int input = seqObj["i"].is<int>() ? seqObj["i"].as<int>() : 0;
    int clockDivider = seqObj["d"].is<int>() ? seqObj["d"].as<int>() : 6;
    if (clockDivider < 0) clockDivider = 6;
    if (clockDivider > 10) clockDivider = 10;
    bool enabled = !seqObj["e"].is<int>() || (seqObj["e"].as<int>() != 0);
    sequence->setType(type);
    sequence->setName(seqObj["n"].is<const char*>() ? seqObj["n"].as<const char*>() : "");
    sequence->setMidiChannel(midiChannel);
    sequence->setPort(port);
    // A stored label wins. When the file has none (legacy patches), keep the
    // label setPort() auto-captured from the device currently at that slot —
    // old patterns gain label binding for free and persist it on next save.
    const char* storedLabel = seqObj["u"].is<const char*>() ? seqObj["u"].as<const char*>() : "";
    if (storedLabel[0]) sequence->setUsbHostLabel(storedLabel);
    sequence->setInput(input);
    sequence->setClockDivider((uint8_t)clockDivider);
    sequence->enableSequence(enabled);

    JsonArray notesArray;
    if (seqObj["s"].is<JsonArray>()) {
        notesArray = seqObj["s"].as<JsonArray>();
    } else {
        notesArray = seqObj["seq"].as<JsonArray>();
    }
    // Backward compat: derive length from note count if not stored
    int length = seqObj["l"] | 0;
    if (length < 1) {
        length = (notesArray.size() + SEQ_BLOCK_SIZE - 1) / SEQ_BLOCK_SIZE;
        if (length < 1) length = 1;
    }
    sequence->setLength(length);
    sequence->clearSequence();

    for (JsonObject noteObj : notesArray) {
        int read = noteObj["r"].is<int>() ? noteObj["r"].as<int>() : noteObj["read"].as<int>();
        int velocity = noteObj["v"].is<int>() ? noteObj["v"].as<int>() : noteObj["vel"].as<int>();
        int length = noteObj["l"].is<int>() ? noteObj["l"].as<int>() : noteObj["len"].as<int>();
        int offset = noteObj["o"].is<int>() ? noteObj["o"].as<int>() : 0;
        bool isActive = velocity > 0;
        RTPEventNotePlus eventNote(midiChannel, false, read, 0);
        eventNote.setEventRead(read);
        eventNote.setEventState(isActive);
        eventNote.setLength(length);
        eventNote.setMicroOffset(offset);
        if (isActive)
            eventNote.setEventVelocity(velocity);
        sequence->addEventNote(eventNote);
    }

    // Ensure the event vector size matches the configured length
    size_t expectedSize = (size_t)sequence->getLength() * SEQ_BLOCK_SIZE;
    if (expectedSize > 0) {
        sequence->resizeSequence(expectedSize);
    }
    
    return true;
}

bool BuitPersistenceManager::parseAndLoadFromDoc(RTPSequencer& sequencer, JsonDocument& doc) {
    JsonArray scenesArray;
    if (doc["sc"].is<JsonArray>()) {
        scenesArray = doc["sc"].as<JsonArray>();
    } else {
        scenesArray = doc["scenes"].as<JsonArray>();
    }
    // Grow the sequencer if the file has more scenes than currently exist
    while (sequencer.getNumScenes() < (int)scenesArray.size()) {
        sequencer.addDynamicScene();
    }
    int numScenes = sequencer.getNumScenes();
    int scenesToProcess = min(numScenes, (int)scenesArray.size());
    
    for (int sceneIdx = 0; sceneIdx < scenesToProcess; sceneIdx++) {
        JsonObject sceneObj = scenesArray[sceneIdx];
        JsonArray sequencesArray;
        if (sceneObj["q"].is<JsonArray>()) {
            sequencesArray = sceneObj["q"].as<JsonArray>();
        } else {
            sequencesArray = sceneObj["sequences"].as<JsonArray>();
        }
        
        // Get a handle to the current scene
        RTPScene* scene = sequencer.getScene(sceneIdx);
        if (!scene) continue;

        // Restore scene-level settings (name + selected sequence)
        String name = sceneObj["n"].is<const char*>() ? sceneObj["n"].as<const char*>()
                    : (sceneObj["name"].is<const char*>() ? sceneObj["name"].as<const char*>() : "");
        if (name.length() > 0) scene->setName(name);
        int selSeq = sceneObj["sel"] | -1;
        if (selSeq >= 0 && selSeq < (int)scene->getSize())
            scene->setSelectedSequence((uint8_t)selSeq);

        // Load sequences for this scene
        int seqCount = min(scene->getSize(), (int)sequencesArray.size());
        for (int seqIdx = 0; seqIdx < seqCount; seqIdx++) {
            JsonObject seqObj = sequencesArray[seqIdx];
            
            // Get reference to the sequence
            RTPEventNoteSequence* sequence = scene->getSequence(static_cast<uint8_t>(seqIdx));
            if (!sequence) continue;
            
            // Re-instantiate the correct subclass when the stored type differs
            int fileType = seqObj["t"].is<int>() ? seqObj["t"].as<int>()
                         : (seqObj["type"].is<int>() ? seqObj["type"].as<int>() : -1);
            if (fileType >= 0 && sequence->getType() != fileType) {
                sequence = scene->recreateSequence((uint8_t)seqIdx, (uint8_t)fileType);
                if (!sequence) continue;
            }
            
            // Load the sequence data
            loadSequenceFromJson(sequence, seqObj);
        }

        // Clear slots the file doesn't define so stale notes don't keep playing
        for (int seqIdx = seqCount; seqIdx < (int)scene->getSize(); seqIdx++) {
            RTPEventNoteSequence* seq = scene->getSequence(seqIdx);
            if (seq) {
                seq->clearSequence();
                seq->enableSequence(false);
                seq->setName("");
            }
        }
    }

    // Clear scenes beyond the file's scene count
    for (int sceneIdx = scenesToProcess; sceneIdx < numScenes; sceneIdx++) {
        RTPScene* scene = sequencer.getScene(sceneIdx);
        if (!scene) continue;
        scene->setName("");
        for (int seqIdx = 0; seqIdx < (int)scene->getSize(); seqIdx++) {
            RTPEventNoteSequence* seq = scene->getSequence(seqIdx);
            if (seq) {
                seq->clearSequence();
                seq->enableSequence(false);
                seq->setName("");
            }
        }
    }
    
    // Restore the selected scene (upper-layer navigation state)
    int selScene = doc["sel"] | -1;
    if (selScene >= 0 && selScene < numScenes)
        sequencer.selectScene((uint8_t)selScene);

    Serial.println("Successfully loaded sequences");
    return true;
}

bool BuitPersistenceManager::loadSequencerFromFile(RTPSequencer& sequencer, const String& fileName) {
    Serial.println("Loading sequences from file: " + fileName);
    
    File file = openFileForRead(fileName);
    if (!file) {
        Serial.println("Failed to open sequences file");
        return false;
    }
    
    char magic[4];
    if (file.readBytes(magic, 4) == 4 && magic[0] == 'R' && magic[1] == 'T' && magic[2] == 'P' && magic[3] == '0') {
        file.seek(0);
        Serial.println("Opened binary file stream");
        return loadSequencerFromBinary(sequencer, file);
    }
    file.seek(0);
    Serial.println("Opened JSON file stream");
    return parseAndLoadSequences(sequencer, file);
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
    return parseAndLoadFromDoc(sequencer, doc);
}

bool BuitPersistenceManager::parseAndLoadSequences(RTPSequencer& sequencer, File& file) {
    sequencer.stopAndCleanSequencer();
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    
    if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return false;
    }
    return parseAndLoadFromDoc(sequencer, doc);
}

bool BuitPersistenceManager::saveSequenceToBinary(RTPEventNoteSequence* sequence, File& file) {
    uint8_t header[10];
    header[0] = sequence->getType();
    header[1] = sequence->getMidiChannel();
    header[2] = (uint8_t)(sequence->getColor() & 0xFFu);
    header[3] = sequence->getLength();
    header[4] = sequence->getInput();
    header[5] = sequence->getPort();
    uint16_t nNotes = (uint16_t)sequence->getEventNoteSequence().size();
    header[6] = (uint8_t)(nNotes & 0xFFu);
    header[7] = (uint8_t)((nNotes >> 8) & 0xFFu);
    header[8] = sequence->isCurrentSequenceEnabled() ? 1 : 0;
    header[9] = sequence->getClockDivider();
    file.write(header, sizeof(header));
    // v4: sequence name
    String name = sequence->getName();
    uint8_t nameLen = (uint8_t)name.length();
    file.write(&nameLen, 1);
    if (nameLen > 0) file.write((const uint8_t*)name.c_str(), nameLen);
    // v5: USB host device label (stable identity for port 5-8 targeting)
    const char* label = sequence->getUsbHostLabel();
    uint8_t labelLen = (uint8_t)strlen(label);
    file.write(&labelLen, 1);
    if (labelLen > 0) file.write((const uint8_t*)label, labelLen);
    for (const RTPEventNotePlus& note : sequence->getEventNoteSequence()) {
        uint32_t low = note.getPackedLow();
        uint32_t high = note.getPackedHigh();
        file.write(&low, sizeof(low));
        file.write(&high, sizeof(high));
    }
    return true;
}

bool BuitPersistenceManager::loadSequenceFromBinary(RTPScene* scene, uint8_t seqIndex, File& file, uint8_t version) {
    uint8_t header[10];
    size_t headerSize;
    if (version >= 3) headerSize = 10;
    else if (version == 2) headerSize = 9;
    else headerSize = 8;
    if (file.readBytes((char*)header, headerSize) != headerSize) return false;

    RTPEventNoteSequence* sequence = scene->getSequence(seqIndex);
    if (!sequence) return false;
    // Re-instantiate the correct subclass when the stored type differs —
    // setType() alone would leave e.g. a DrumSequence playing as a drum.
    if (sequence->getType() != header[0]) {
        sequence = scene->recreateSequence(seqIndex, header[0]);
        if (!sequence) return false;
    }
    sequence->setType(header[0]);
    sequence->setMidiChannel(header[1]);
    sequence->setColor(header[2]);
    sequence->setLength(header[3]);
    sequence->setInput(header[4]);
    sequence->setPort(header[5]);
    uint16_t nNotes = (uint16_t)header[6] | ((uint16_t)header[7] << 8);
    bool enabled = (version >= 2) ? (header[8] != 0) : true;
    uint8_t clockDivider = (version >= 3) ? header[9] : 6;
    if (clockDivider > 10) clockDivider = 6;
    sequence->setClockDivider(clockDivider);
    sequence->enableSequence(enabled);
    // v4: sequence name (stored between header and notes)
    if (version >= 4) {
        uint8_t nameLen;
        if (file.readBytes((char*)&nameLen, 1) != 1) return false;
        char nameBuf[256] = {0};
        if (nameLen > 0 && file.readBytes(nameBuf, nameLen) != nameLen) return false;
        nameBuf[nameLen] = '\0';
        sequence->setName(String(nameBuf));
    } else {
        sequence->setName("");
    }
    // v5: USB host device label. A stored label wins; when absent (legacy
    // files or empty), keep the label setPort() auto-captured from the
    // device currently at that slot.
    if (version >= 5) {
        uint8_t labelLen;
        if (file.readBytes((char*)&labelLen, 1) != 1) return false;
        char labelBuf[48] = {0};
        uint8_t readLen = labelLen < sizeof(labelBuf) - 1 ? labelLen : sizeof(labelBuf) - 1;
        if (readLen > 0 && file.readBytes(labelBuf, readLen) != readLen) return false;
        if (labelLen > readLen && !file.seek(file.position() + (labelLen - readLen))) return false;
        labelBuf[readLen] = '\0';
        if (labelBuf[0]) sequence->setUsbHostLabel(labelBuf);
    }
    sequence->clearSequence();
    sequence->resizeSequence(nNotes);
    auto& notes = sequence->getEventNoteSequence();
    for (uint16_t i = 0; i < nNotes; i++) {
        uint32_t low, high;
        if (file.readBytes((char*)&low, sizeof(low)) != sizeof(low)) return false;
        if (file.readBytes((char*)&high, sizeof(high)) != sizeof(high)) return false;
        notes[i].setPacked(low, high);
    }
    return true;
}

// Skip over a sequence block without loading it (file has more sequences
// than the scene has slots — keeps the stream aligned for the next scene).
bool BuitPersistenceManager::skipSequenceInBinary(File& file, uint8_t version) {
    uint8_t header[10];
    size_t headerSize;
    if (version >= 3) headerSize = 10;
    else if (version == 2) headerSize = 9;
    else headerSize = 8;
    if (file.readBytes((char*)header, headerSize) != headerSize) return false;
    uint16_t nNotes = (uint16_t)header[6] | ((uint16_t)header[7] << 8);
    if (version >= 4) {
        uint8_t nameLen;
        if (file.readBytes((char*)&nameLen, 1) != 1) return false;
        if (!file.seek(file.position() + nameLen)) return false;
    }
    if (version >= 5) {
        uint8_t labelLen;
        if (file.readBytes((char*)&labelLen, 1) != 1) return false;
        if (!file.seek(file.position() + labelLen)) return false;
    }
    return file.seek(file.position() + (uint32_t)nNotes * 8);
}

bool BuitPersistenceManager::saveSequencerToBinary(const RTPSequencer& sequencer, const String& fileName) {
    File file = openFileForWrite(fileName);
    if (!file) {
        Serial.println("Error opening file for binary write");
        return false;
    }
    uint8_t header[8];
    header[0] = 'R'; header[1] = 'T'; header[2] = 'P'; header[3] = '0';
    header[4] = 5; // version
    header[5] = (uint8_t)sequencer.getNumScenes();
    header[6] = (uint8_t)SCENE_BLOCK_SIZE;
    header[7] = (uint8_t)sequencer.getSelectScene();
    file.write(header, sizeof(header));
    int numScenes = sequencer.getNumScenes();
    for (int i = 0; i < numScenes; i++) {
        RTPScene* scene = sequencer.getScene(i);
        if (!scene) continue;
        String name = scene->getName();
        uint8_t nameLen = (uint8_t)name.length();
        if (nameLen > 255) nameLen = 255;
        file.write(&nameLen, 1);
        file.write((const uint8_t*)name.c_str(), nameLen);
        uint8_t selSeq = scene->getSelectedSequenceIndex();
        file.write(&selSeq, 1);
        uint8_t nSeq = scene->getSize();
        file.write(&nSeq, 1);
        for (uint8_t j = 0; j < nSeq; j++) {
            RTPEventNoteSequence* sequence = scene->getSequence(j);
            if (!sequence) continue;
            saveSequenceToBinary(sequence, file);
        }
    }
    file.close();
    Serial.println("Saved binary sequences");
    return true;
}

bool BuitPersistenceManager::loadSequencerFromBinary(RTPSequencer& sequencer, File& file) {
    sequencer.stopAndCleanSequencer();
    uint8_t header[8];
    if (file.readBytes((char*)header, sizeof(header)) != sizeof(header)) return false;
    if (header[0] != 'R' || header[1] != 'T' || header[2] != 'P' || header[3] != '0') return false;
    uint8_t version = header[4];
    if (version < 1 || version > 5) return false;
    uint8_t nScenes = header[5];
    // Grow the sequencer if the file has more scenes than currently exist
    while (sequencer.getNumScenes() < (int)nScenes) {
        sequencer.addDynamicScene();
    }
    int numScenes = min((int)nScenes, (int)sequencer.getNumScenes());
    for (int i = 0; i < numScenes; i++) {
        RTPScene* scene = sequencer.getScene(i);
        if (!scene) continue;
        uint8_t nameLen;
        if (file.readBytes((char*)&nameLen, 1) != 1) return false;
        if (nameLen > 0) {
            char nameBuf[256] = {0};
            if (file.readBytes(nameBuf, nameLen) != nameLen) return false;
            nameBuf[nameLen] = '\0';
            scene->setName(String(nameBuf));
        }
        uint8_t selSeq = 0;
        if (version >= 2) {
            if (file.readBytes((char*)&selSeq, 1) != 1) return false;
            if (selSeq < scene->getSize())
                scene->setSelectedSequence(selSeq);
        }
        (void)selSeq;
        uint8_t nSeq;
        if (file.readBytes((char*)&nSeq, 1) != 1) return false;
        int seqCount = min((int)nSeq, (int)scene->getSize());
        for (int j = 0; j < seqCount; j++) {
            if (!loadSequenceFromBinary(scene, j, file, version)) return false;
        }
        // Skip sequences beyond capacity so the stream stays aligned
        for (int j = seqCount; j < (int)nSeq; j++) {
            if (!skipSequenceInBinary(file, version)) return false;
        }
        // Clear slots the file doesn't define so stale notes don't keep playing
        for (int j = seqCount; j < (int)scene->getSize(); j++) {
            RTPEventNoteSequence* seq = scene->getSequence(j);
            if (seq) {
                seq->clearSequence();
                seq->enableSequence(false);
                seq->setName("");
            }
        }
    }
    // Clear scenes beyond the file's scene count
    for (int i = numScenes; i < sequencer.getNumScenes(); i++) {
        RTPScene* scene = sequencer.getScene(i);
        if (!scene) continue;
        scene->setName("");
        for (int j = 0; j < (int)scene->getSize(); j++) {
            RTPEventNoteSequence* seq = scene->getSequence(j);
            if (seq) {
                seq->clearSequence();
                seq->enableSequence(false);
                seq->setName("");
            }
        }
    }
    if (version >= 2) {
        uint8_t selScene = header[7];
        if (selScene < (uint8_t)sequencer.getNumScenes())
            sequencer.selectScene(selScene);
    }
    Serial.println("Successfully loaded binary sequences");
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