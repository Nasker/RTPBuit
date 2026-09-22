#include "Midi/MidiInputDispatcher.hpp"
#include "RTPSequencer.h"
#include "RTPScene.h"
#include "RTPEventNoteSequence.h"
#include "Managers/RecordingManager.hpp"
#include "LivePlayOrchestrator.hpp"

MidiInputDispatcher::MidiInputDispatcher(RTPSequencer& sequencer,
    RecordingManager& recordingManager, LivePlayOrchestrator& orchestrator)
    : _sequencer(sequencer), _recordingManager(recordingManager),
      _orchestrator(orchestrator) {}

void MidiInputDispatcher::noteOn(uint8_t channel, uint8_t note, uint8_t velocity,
                                 uint8_t srcPort, uint8_t srcDevice) {
    // channel is plumbing for a future per-lane input-channel filter
    (void)channel;
    if (note >= 128) return;
    RTPScene* scene = _sequencer.getScene(_sequencer.getCurrentScene());
    if (!scene) return;
    uint8_t selected = scene->getSelectedSequenceIndex();
    _noteOwners[note] = 0;
    for (uint8_t i = 0; i < scene->getSize(); i++) {
        RTPEventNoteSequence* seq = scene->getSequence(i);
        if (!seq) continue;
        if (!seq->acceptsInput(srcPort, srcDevice, i == selected)) continue;
        seq->thruNoteOn(note, velocity);
        if (i < 16) _noteOwners[note] |= (uint16_t)(1u << i);
        // Recording is single-lane: the recorder is bound to the selected
        // sequence's clock divider and length.
        if (i == selected && _recordingManager.isRecording())
            _orchestrator.recorderNoteOn(note, velocity);
    }
}

void MidiInputDispatcher::noteOff(uint8_t channel, uint8_t note,
                                  uint8_t srcPort, uint8_t srcDevice) {
    (void)channel;
    if (note >= 128) return;
    RTPScene* scene = _sequencer.getScene(_sequencer.getCurrentScene());
    if (!scene) return;
    uint8_t selected = scene->getSelectedSequenceIndex();
    uint16_t owners = _noteOwners[note];
    _noteOwners[note] = 0;
    for (uint8_t i = 0; i < scene->getSize(); i++) {
        RTPEventNoteSequence* seq = scene->getSequence(i);
        if (!seq) continue;
        // Thru target: whoever got the note-on, even if the lane no longer
        // accepts this source — skipping it would leave a stuck note on its
        // output. Untracked offs fall back to the input filter.
        bool thru = (owners != 0)
            ? (i < 16 && (owners & (uint16_t)(1u << i)))
            : seq->acceptsInput(srcPort, srcDevice, i == selected);
        if (thru) seq->thruNoteOff(note);
        if (i == selected && _recordingManager.isRecording())
            _orchestrator.recorderNoteOff(note);
    }
}

void MidiInputDispatcher::controlChange(uint8_t channel, uint8_t controller,
                                        uint8_t value, uint8_t srcPort,
                                        uint8_t srcDevice) {
    (void)channel;
    RTPScene* scene = _sequencer.getScene(_sequencer.getCurrentScene());
    if (!scene) return;
    uint8_t selected = scene->getSelectedSequenceIndex();
    for (uint8_t i = 0; i < scene->getSize(); i++) {
        RTPEventNoteSequence* seq = scene->getSequence(i);
        if (!seq) continue;
        if (!seq->acceptsInput(srcPort, srcDevice, i == selected)) continue;
        seq->thruCC(controller, value);
    }
}
