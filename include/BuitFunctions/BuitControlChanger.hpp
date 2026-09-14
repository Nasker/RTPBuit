#pragma once

#include "Arduino.h"
#include "ControlCommand.h"
#include "constants.h"

class MidiRouter;

class BuitControlChanger {
    int _ID;
    int _lastValues[3];
    int _currentValues[3];
    bool _enabled;
public:
    BuitControlChanger(int ID);
    void updateAndSend(ControlCommand command);
    void enable();
    bool isEnabled();
    void disable();
    static void setRouter(MidiRouter* router);
private:
    static MidiRouter* _router;
    void send(ControlCommand command);
};