#include "BuitPersistenceManager.hpp"

BuitPersistenceManager::BuitPersistenceManager(){}

BuitPersistenceManager::~BuitPersistenceManager(){}

void BuitPersistenceManager::save(RTPEventNoteSequence eventNoteSequence){
    String noteSeqString;
    StaticJsonDocument<1536> doc;
    doc["type"] = eventNoteSequence.getType();
    doc["ch"] = eventNoteSequence.getMidiChannel();
    JsonArray seq = doc.createNestedArray("seq");
    for (RTPEventNotePlus eventNote : eventNoteSequence.getEventNoteSequence()){
        JsonObject note = seq.createNestedObject();
        note["read"] = eventNote.getEventRead();
        note["vel"] = eventNote.eventState() ? eventNote.getEventVelocity() : 0;
        note["len"] = eventNote.getLength();
    }
    serializeJsonPretty(doc, noteSeqString);
    Serial.printf("%s\n", noteSeqString.c_str());
    //return noteSeqString;
}

void BuitPersistenceManager::load(RTPEventNoteSequence &eventNoteSequence){

}
