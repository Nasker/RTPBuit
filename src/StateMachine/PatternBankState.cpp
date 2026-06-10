#include "Arduino.h"
#include "PatternBankState.h"
#include "RTPNeoTrellis.hpp"
#include "constants.h"

PatternBankState::PatternBankState(BuitStateMachine& machine, BuitDevicesManager& devices, PatternBankMode mode)
    : BuitState(devices), _buitMachine(machine), _mode(mode) {
}

// ── Helpers ─────────────────────────────────────────────────────────────────

String PatternBankState::buildFileName(uint8_t page, uint8_t slot) const {
    // Format: p00s00.json — zero-padded page and slot
    char buf[16];
    snprintf(buf, sizeof(buf), "p%02ds%02d.json", page, slot);
    return String(buf);
}

void PatternBankState::resetState() {
    _inSlotSelect    = false;
    _selectedPage    = 0;
    _awaitingConfirm = false;
    _confirmSlot     = 0;
}

void PatternBankState::renderPageGrid() {
    _devices.clearTrellis();
    for (uint8_t i = 0; i < PATTERN_BANK_PAGES; i++) {
        _devices.setTrellisButtonColor(i, RTPNeoTrellis::colorForPage(i));
    }
    _devices.showTrellis();
    const char* modeLabel = (_mode == PatternBankMode::Load) ? "Load" : "Save";
    _devices.printToScreen(modeLabel, "Select Page", "");
}

void PatternBankState::renderSlotGrid(uint8_t page) {
    _devices.clearTrellis();
    for (uint8_t i = 0; i < PATTERN_BANK_SLOTS; i++) {
        bool exists = _devices.patternFileExists(buildFileName(page, i));
        _devices.setTrellisButtonColor(i, RTPNeoTrellis::colorForSlot(page, exists));
    }
    _devices.showTrellis();
    const char* modeLabel = (_mode == PatternBankMode::Load) ? "Load" : "Save";
    char slotLabel[12];
    snprintf(slotLabel, sizeof(slotLabel), "Page %d", page);
    _devices.printToScreen(modeLabel, slotLabel, "Select Slot");
}

// ── Public entry ─────────────────────────────────────────────────────────────

void PatternBankState::onEnter() {
    resetState();
    renderPageGrid();
}

// ── Button / trellis ─────────────────────────────────────────────────────────

void PatternBankState::trellisPressed(ControlCommand command) {
    uint8_t pad = (uint8_t)command.value;
    
    if (!_inSlotSelect) {
        // Page select
        if (pad >= PATTERN_BANK_PAGES) return;
        _selectedPage    = pad;
        _inSlotSelect    = true;
        _awaitingConfirm = false;
        renderSlotGrid(_selectedPage);
        return;
    }

    // Slot select
    if (pad >= PATTERN_BANK_SLOTS) return;
    String fileName = buildFileName(_selectedPage, pad);

    if (_mode == PatternBankMode::Load) {
        _devices.loadSequencer(fileName);
        _buitMachine.setState(_buitMachine.getSceneEditState());

    } else {
        // Save mode: require confirmation on existing files
        bool exists = _devices.patternFileExists(fileName);
        if (exists && (!_awaitingConfirm || _confirmSlot != pad)) {
            // First press on occupied slot — ask for confirmation
            _awaitingConfirm = true;
            _confirmSlot     = pad;
            char msg[16];
            snprintf(msg, sizeof(msg), "Slot %d", pad);
            _devices.printToScreen("Overwrite?", msg, "Press again");
            return;
        }
        // Either empty slot (write immediately) or confirmed overwrite
        _devices.saveSequencer(fileName);
        resetState();
        _buitMachine.setState(_buitMachine.getSceneEditState());
    }
}

void PatternBankState::trellisReleased(ControlCommand command) {}

// ── Navigation ───────────────────────────────────────────────────────────────

void PatternBankState::singleClick() {
    if (_inSlotSelect) {
        // Step back to page select
        _inSlotSelect    = false;
        _awaitingConfirm = false;
        renderPageGrid();
    }
}

void PatternBankState::doubleClick() {
    resetState();
    _buitMachine.setState(_buitMachine.getSceneEditState());
}

// ── Unused overrides ─────────────────────────────────────────────────────────

void PatternBankState::tripleClick()                               {}
void PatternBankState::longClick()                                 {}
void PatternBankState::rotaryTurned(ControlCommand command)        {}
void PatternBankState::threeAxisChanged(ControlCommand command)    {}
void PatternBankState::sequencerCallback(ControlCommand command)   {}
void PatternBankState::midiNote(ControlCommand command)            {}
void PatternBankState::midiNoteOff(ControlCommand command)         {}
void PatternBankState::midiCC(ControlCommand command)              {}
