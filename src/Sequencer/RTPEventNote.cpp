#include "RTPEventNote.hpp"

RTPEventNote::RTPEventNote(bool state, uint8_t note){
    _low = 0;
    _high = 0;
    setEventState(state);
    setEventNote(note);
    setEventVelocity(100);
    setEventRead(0);
    setLength(1);
    setTimeToLive(1);
}

RTPEventNote::RTPEventNote(bool state, uint8_t note, uint8_t velocity){
    _low = 0;
    _high = 0;
    setEventState(state);
    setEventNote(note);
    setEventVelocity(velocity);
    setEventRead(0);
    setLength(1);
    setTimeToLive(1);
}

bool RTPEventNote::eventState(){
    return (_high >> 16) & 0x1u;
}

// Const version for JSON serialization
bool RTPEventNote::eventState() const {
    return (_high >> 16) & 0x1u;
}

uint8_t RTPEventNote::getEventNote(){
    return _low & 0xFFu;
}

// Const version for JSON serialization
uint8_t RTPEventNote::getEventNote() const {
    return _low & 0xFFu;
}

uint8_t RTPEventNote::getEventRead(){
    return (_low >> 8) & 0xFFu;
}

// Const version for JSON serialization
uint8_t RTPEventNote::getEventRead() const {
    return (_low >> 8) & 0xFFu;
}

uint8_t RTPEventNote::getEventVelocity(){
    return (_low >> 16) & 0xFFu;
}

// Const version for JSON serialization
uint8_t RTPEventNote::getEventVelocity() const {
    return (_low >> 16) & 0xFFu;
}

void RTPEventNote::setEventState(bool state){
    _high = (_high & ~(0x1u << 16)) | (state ? (0x1u << 16) : 0);
}

void RTPEventNote::setEventNote(uint8_t note){
    _low = (_low & ~0xFFu) | note;
}

void RTPEventNote::setEventRead(uint8_t read){
    _low = (_low & ~(0xFFu << 8)) | (read << 8);
}

void RTPEventNote::setEventVelocity(uint8_t velocity){
    _low = (_low & ~(0xFFu << 16)) | (velocity << 16);
}

void RTPEventNote::switchState(){
    _high ^= (0x1u << 16);
}

uint8_t RTPEventNote::getMidiChannel(){
    return _high & 0xFu;
}

// Const version for JSON serialization
uint8_t RTPEventNote::getMidiChannel() const {
    return _high & 0xFu;
}

void RTPEventNote::setMidiChannel(uint8_t midiChannel){
    _high = (_high & ~0xFu) | (midiChannel & 0xFu);
}

uint8_t RTPEventNote::getLength() const {
    return (_low >> 24) & 0xFu;
}

void RTPEventNote::setLength(uint8_t length){
    _low = (_low & ~(0xFu << 24)) | ((length & 0xFu) << 24);
    setTimeToLive(length);
}

uint8_t RTPEventNote::getTimeToLive() const {
    return (_low >> 28) & 0xFu;
}

void RTPEventNote::setTimeToLive(uint8_t timeToLive){
    _low = (_low & ~(0xFu << 28)) | ((timeToLive & 0xFu) << 28);
}

MidiPort RTPEventNote::getDestPort() const {
    return static_cast<MidiPort>((_high >> 4) & 0xFu);
}

void RTPEventNote::setDestPort(MidiPort port){
    _high = (_high & ~(0xFu << 4)) | ((static_cast<uint8_t>(port) & 0xFu) << 4);
}

uint8_t RTPEventNote::getUsbHostIndex() const {
    return (_high >> 8) & 0xFFu;
}

void RTPEventNote::setUsbHostIndex(uint8_t idx){
    _high = (_high & ~(0xFFu << 8)) | (idx << 8);
}

bool RTPEventNote::isLiteralPitch() const {
    return (_high >> 17) & 0x1u;
}

void RTPEventNote::setLiteralPitch(bool literal){
    _high = (_high & ~(0x1u << 17)) | (literal ? (0x1u << 17) : 0);
}
