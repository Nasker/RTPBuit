#pragma once

#include <cstdint>

class RTPSequencer;
class RecordingManager;
class LivePlayOrchestrator;

/**
 * @brief Per-lane fan-out for incoming MIDI, independent of the UI state
 *
 * Each sequence picks its own input source (INPUT_PORT parameter); an
 * incoming event is played through the output of every lane that listens to
 * that source, and recorded into the selected lane while a take is armed.
 */
class MidiInputDispatcher {
    RTPSequencer& _sequencer;
    RecordingManager& _recordingManager;
    LivePlayOrchestrator& _orchestrator;
    // Bit i = lane i received the note-on for that note number. The note-off
    // must reach the same lanes even if selection or input routing changed
    // meanwhile — otherwise the previous owner's output holds a stuck note.
    uint16_t _noteOwners[128] = {0};

public:
    MidiInputDispatcher(RTPSequencer& sequencer, RecordingManager& recordingManager,
                        LivePlayOrchestrator& orchestrator);

    void noteOn(uint8_t channel, uint8_t note, uint8_t velocity,
                uint8_t srcPort, uint8_t srcDevice);
    void noteOff(uint8_t channel, uint8_t note,
                 uint8_t srcPort, uint8_t srcDevice);
    void controlChange(uint8_t channel, uint8_t controller, uint8_t value,
                       uint8_t srcPort, uint8_t srcDevice);
};
