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
    int getEventNote();
    int getEventRead();
    int getEventVelocity();
    void setEventState(bool state);
    void setEventNote(int note);
    void setEventRead(int read);
    void setEventVelocity(int velocity);
    void switchState();
};