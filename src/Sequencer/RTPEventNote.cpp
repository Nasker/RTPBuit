
#include "RTPEventNote.h"

RTPEventNote::RTPEventNote(bool state, int note){
	_state = state;
	_note = note;
	_velocity = 100;
	_read = 0;
}

RTPEventNote::RTPEventNote(bool state, int note, int velocity){
	_state = state;
	_note = note;
	_velocity = velocity;
	_read = 0;
}

bool RTPEventNote::eventState(){
	return _state;
}

int RTPEventNote::getEventNote(){
	return _note;
}

int RTPEventNote::getEventRead(){
	return _read;
}

int RTPEventNote::getEventVelocity(){
	return _velocity;
}

void RTPEventNote::setEventState(bool state){
	_state = state;
}

void RTPEventNote::setEventNote(int note){
	_note = note;
}

void RTPEventNote::setEventRead(int read){
	_read = read;
}

void RTPEventNote::setEventVelocity(int velocity){
	_velocity = velocity;
}

void RTPEventNote::switchState(){
	_state = !_state;
}

