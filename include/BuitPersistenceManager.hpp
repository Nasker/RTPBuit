#pragma once

#include "RTPEventNoteSequence.h"

class BuitPersistenceManager{
public:
    BuitPersistenceManager();
    ~BuitPersistenceManager();
    void save(RTPEventNoteSequence eventNoteSequence);
    void load(RTPEventNoteSequence &eventNoteSequence);
};