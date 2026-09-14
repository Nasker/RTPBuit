#include "BuitControlChanger.hpp"
#include "Midi/MidiRouter.hpp"
#include "Midi/MidiMessage.hpp"

MidiRouter* BuitControlChanger::_router = nullptr;

void BuitControlChanger::setRouter(MidiRouter* router) {
    _router = router;
}

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
    if (_router) {
        MidiMessage msg { MidiMessage::ControlChange,
                          static_cast<uint8_t>(_ID + 1),
                          static_cast<uint8_t>(command.commandType),
                          static_cast<uint8_t>(command.value),
                          MidiPort::INTERNAL };
        _router->route(msg);
    }
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