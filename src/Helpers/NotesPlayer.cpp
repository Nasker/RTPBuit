#include "NotesPlayer.hpp"

NotesPlayer::NotesPlayer(){
    _notesQueue = queue<RTPEventNotePlus>();
    _ringingNotes = vector<std::map<int, RTPEventNotePlus>>(16);
}

void NotesPlayer::queueNote(RTPEventNotePlus note){
    _notesQueue.push(note);
}

void NotesPlayer::playNotes(){
    while(!_notesQueue.empty()){
        RTPEventNotePlus note = _notesQueue.front();
        _notesQueue.pop();
        auto ans = _ringingNotes[note.getMidiChannel() - 1].insert( std::pair<int, RTPEventNotePlus>(note.getEventNote(), note) );
        if(ans.second) // play note only if it was not already playing
            note.playNoteOn();
        else // if it was already playing, update its length
            ans.first->second.setLength(note.getLength());
    }
}

void NotesPlayer::decreaseTimeToLive(){
    std::map<int, RTPEventNotePlus>::iterator it;
    for(size_t i = 0; i < _ringingNotes.size(); i++){
        for(it = _ringingNotes[i].begin(); it != _ringingNotes[i].end();){
            if(!it->second.decreaseTimeToLive()){
                it->second.playNoteOff();
                _ringingNotes[i].erase(it++);
            } 
            else
                ++it;  
        }   
    }
}

bool NotesPlayer::killThatNote(RTPEventNotePlus note){
    std::map<int, RTPEventNotePlus>::iterator it;
    it = _ringingNotes[note.getMidiChannel()-1].find(note.getEventNote());
    if(it != _ringingNotes[note.getMidiChannel()-1].end()){
        it->second.playNoteOff();
        _ringingNotes[note.getMidiChannel()-1].erase(it);
        return true; 
    }
    return false;
}

void NotesPlayer::killAllNotes(){
    std::map<int, RTPEventNotePlus>::iterator it;
    for(size_t i = 0; i < _ringingNotes.size(); i++){
        for(it = _ringingNotes[i].begin(); it != _ringingNotes[i].end(); it++)
            it->second.playNoteOff();
        _ringingNotes[i].clear();
    }
    while(!_notesQueue.empty()){
        RTPEventNotePlus note = _notesQueue.front();
        note.playNoteOff();
        _notesQueue.pop();
    }
}