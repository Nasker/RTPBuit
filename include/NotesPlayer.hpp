#pragma once

#include "RTPEventNotePlus.h"
#include "Arduino.h"
#include <map>
#include <queue>
#include <vector>
//#include "RTPMainUnit.hpp"

using namespace std;

class NotesPlayer {
    //static RTPMainUnit* mainUnit;
    queue<RTPEventNotePlus> _notesQueue; 
    vector<std::map<int, RTPEventNotePlus>> _ringingNotes;
public:
    NotesPlayer();
    //void connectMainUnit(RTPMainUnit* mainUnit);
    void queueNote(RTPEventNotePlus note);
    void playNotes();
    void decreaseTimeToLive();
    bool killThatNote(RTPEventNotePlus note);
    void killAllNotes();
private:
    int keyToNote(RTPEventNotePlus note);
};