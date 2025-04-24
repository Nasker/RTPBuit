#include "RTPEventNote.hpp"

RTPEventNote::RTPEventNote(bool state, uint8_t note){
	_state = state;
	_note = note;
	_velocity = 100;
	_read = 0;
}

RTPEventNote::RTPEventNote(bool state, uint8_t note, uint8_t velocity){
	_state = state;
	_note = note;
	_velocity = velocity;
	_read = 0;
}

bool RTPEventNote::eventState(){
	return _state;
}

// Const version for JSON serialization
bool RTPEventNote::eventState() const {
	return _state;
}

uint8_t RTPEventNote::getEventNote(){
	return _note;
}

// Const version for JSON serialization
uint8_t RTPEventNote::getEventNote() const {
	return _note;
}

uint8_t RTPEventNote::getEventRead(){
	return _read;
}

// Const version for JSON serialization
uint8_t RTPEventNote::getEventRead() const {
	return _read;
}

uint8_t RTPEventNote::getEventVelocity(){
	return _velocity;
}

// Const version for JSON serialization
uint8_t RTPEventNote::getEventVelocity() const {
	return _velocity;
}

void RTPEventNote::setEventState(bool state){
	_state = state;
}

void RTPEventNote::setEventNote(uint8_t note){
	_note = note;
}

void RTPEventNote::setEventRead(uint8_t read){
	_read = read;
}

void RTPEventNote::setEventVelocity(uint8_t velocity){
	_velocity = velocity;
}

void RTPEventNote::switchState(){
	_state = !_state;
}
