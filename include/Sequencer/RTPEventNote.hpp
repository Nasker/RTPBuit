#pragma once 

class RTPEventNote{
    bool _state;
    int _note;
    int _read;
    int _velocity;

	public:
    RTPEventNote(bool state, int note);
    RTPEventNote(bool state, int note, int velocity);
    bool eventState();
    bool eventState() const;  // Const version for JSON serialization
    int getEventNote();
    int getEventNote() const;  // Const version for JSON serialization
    int getEventRead();
    int getEventRead() const;  // Const version for JSON serialization
    int getEventVelocity();
    int getEventVelocity() const;  // Const version for JSON serialization
    void setEventState(bool state);
    void setEventNote(int note);
    void setEventRead(int read);
    void setEventVelocity(int velocity);
    void switchState();
};