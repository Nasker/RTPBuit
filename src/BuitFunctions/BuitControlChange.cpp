#include "BuitControlChanger.hpp"

BuitControlChanger::BuitControlChanger(int ID){
    _ID = ID;
    for(int i=0; i < 3; i++){
        _lastValues[i] = 0;
        _currentValues[i] = 0;
    }
    _enabled = false;
}

void BuitControlChanger::updateAndSend(ControlCommand command){
    _currentValues[command.commandType] = command.value;
    if (_lastValues[command.commandType] != _currentValues[command.commandType]){
        send(command);
        _lastValues[command.commandType] = _currentValues[command.commandType];
    }
}

void BuitControlChanger::send(ControlCommand command){
    usbMIDI.sendControlChange(command.commandType, command.value, _ID + 1);
}

void BuitControlChanger::enable(){
    _enabled = true;
}

bool BuitControlChanger::isEnabled(){
    return _enabled;
}

void BuitControlChanger::disable(){
    _enabled = false;
}