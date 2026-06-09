#pragma once

#include "BuitStateMachine.h"

enum class PatternBankMode { Load, Save };

class PatternBankState : public BuitState {
    BuitStateMachine& _buitMachine;
    PatternBankMode   _mode;

    // Sub-mode within state
    bool    _inSlotSelect = false;  // false=page select, true=slot select
    uint8_t _selectedPage = 0;

    // Save confirmation: waiting for second press on same slot
    bool    _awaitingConfirm = false;
    uint8_t _confirmSlot     = 0;

public:
    PatternBankState(BuitStateMachine& machine, BuitDevicesManager& devices, PatternBankMode mode);

    void enter();  // Call when transitioning into this state

    void singleClick()                          override;
    void doubleClick()                          override;
    void tripleClick()                          override;
    void longClick()                            override;
    void rotaryTurned(ControlCommand command)   override;
    void threeAxisChanged(ControlCommand command) override;
    void trellisPressed(ControlCommand command) override;
    void trellisReleased(ControlCommand command) override;
    void sequencerCallback(ControlCommand command) override;
    void midiNote(ControlCommand command)       override;
    void midiNoteOff(ControlCommand command)    override;
    void midiCC(ControlCommand command)         override;

private:
    String buildFileName(uint8_t page, uint8_t slot) const;
    void renderPageGrid();
    void renderSlotGrid(uint8_t page);
    void resetState();
};
