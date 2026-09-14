#pragma once

#include "Arduino.h"
#include "Interfaces/IDisplay.hpp"
#include "Interfaces/IInputDevice.hpp"
#include "Interfaces/ISequencer.hpp"
#include "Interfaces/IClockGenerator.hpp"
#include "RTPSequencer.h"
#include "Managers/RecordingManager.hpp"
#include "Structs.h"

class UsbHostManager;

class SequenceSettingsPresenter {
    IDisplay& _display;
    IButtonMatrix& _trellis;
    ISequencer& _sequencer;
    RTPSequencer& _concreteSequencer;
    RecordingManager& _recordingManager;
    UsbHostManager* _usbHostManager = nullptr;

public:
    SequenceSettingsPresenter(IDisplay& display, IButtonMatrix& trellis,
                              ISequencer& sequencer, RTPSequencer& concreteSequencer,
                              RecordingManager& recordingManager);

    void setUsbHostManager(UsbHostManager* mgr) { _usbHostManager = mgr; }

    void presentSequenceSettings();
    void showSequence();
    void presentScene();
    void presentSceneSettings(int8_t focusedPad = -1);
    SequenceDisplayState getSequenceDisplayState();
    void presentTransport(float bpm, SyncMode syncMode);

private:
    String resolvePortDisplayName(int paramValue, const char* names[]);
    void writeTransportPage();
};
