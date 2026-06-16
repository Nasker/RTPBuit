#pragma once

#include "ControlCommand.h"

// Forward declaration
class BuitDevicesManager;

/**
 * @brief Interface for state machine states
 * 
 * This interface provides a focused approach to state handling,
 * allowing states to implement only the methods they need.
 */
class IState {
protected:
    BuitDevicesManager& _devices;

public:
    explicit IState(BuitDevicesManager& devices) : _devices(devices) {}
    virtual ~IState() = default;

    // State lifecycle
    virtual void onEnter() {}
    virtual void onExit() {}

    // Input handling - states can override only what they need
    virtual void onSingleClick() {}
    virtual void onDoubleClick() {}
    virtual void onTripleClick() {}
    virtual void onLongClick() {}
    virtual void onRotaryTurned(ControlCommand command) {}
    virtual void onThreeAxisChanged(ControlCommand command) {}
    virtual void onTrellisPressed(ControlCommand command) {}
    virtual void onTrellisReleased(ControlCommand command) {}
    virtual void onSequencerCallback(ControlCommand command) {}
    virtual void onMidiNote(ControlCommand command) {}
    virtual void onMidiNoteOff(ControlCommand command) {}
    virtual void onMidiCC(ControlCommand command) {}

    // State identification
    virtual String getName() const = 0;
    virtual bool canTransitionTo(const IState& targetState) const { return true; }
};
