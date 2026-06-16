#pragma once

#include "Framework/Assert.hpp"
#include "Mocks/MockMidiOutput.hpp"

/**
 * Tests for MockMidiOutput correctness and the live-play MIDI patterns used by
 * BassSequence / MonoSequence (silence, legato retrigger, roll).
 *
 * BassSequence / MonoSequence cannot be instantiated in the test environment
 * without RTPLib's NotesPlayer + MusicManager, so the three live-play patterns
 * are reproduced here as thin helpers that call IMidiOutput directly — the same
 * way the real sequence classes do after Phase 2.3.
 */
namespace TestMidiOutput {

// ---------------------------------------------------------------------------
// Helpers mirroring BassSequence / MonoSequence live-play patterns
// ---------------------------------------------------------------------------

struct LivePlayer {
    IMidiOutput* midi = nullptr;
    uint8_t channel   = 1;
    uint8_t note      = 0;
    bool    sounding  = false;
    uint8_t velocity  = 90;

    void silence() {
        if (!sounding) return;
        if (midi) midi->sendNoteOff(note, 0, channel);
        sounding = false;
    }

    // Legato: new NoteOn BEFORE old NoteOff
    void retrigger(uint8_t targetNote) {
        uint8_t oldNote   = note;
        bool    wasSounding = sounding;
        if (midi) midi->sendNoteOn(targetNote, velocity, channel);
        if (wasSounding && oldNote != targetNote)
            if (midi) midi->sendNoteOff(oldNote, 0, channel);
        note     = targetNote;
        sounding = true;
    }

    // Roll: hard re-trigger (NoteOff then NoteOn same note)
    void roll() {
        if (!sounding) return;
        if (midi) midi->sendNoteOff(note, 0, channel);
        if (midi) midi->sendNoteOn(note, velocity, channel);
    }
};

// ---------------------------------------------------------------------------
// 1. MockMidiOutput records NoteOn events
// ---------------------------------------------------------------------------
void testMockRecordsNoteOn() {
    MockMidiOutput mock;
    mock.sendNoteOn(60, 90, 1);
    ASSERT_TRUE(mock.hasNoteOn(60));
    ASSERT_TRUE(mock.hasNoteOn(60, 1));
    ASSERT_FALSE(mock.hasNoteOn(61));
    ASSERT_EQ(1, mock.noteOnCount());
}

// ---------------------------------------------------------------------------
// 2. MockMidiOutput records NoteOff events
// ---------------------------------------------------------------------------
void testMockRecordsNoteOff() {
    MockMidiOutput mock;
    mock.sendNoteOff(60, 0, 1);
    ASSERT_TRUE(mock.hasNoteOff(60));
    ASSERT_FALSE(mock.hasNoteOff(61));
    ASSERT_EQ(1, mock.noteOffCount());
}

// ---------------------------------------------------------------------------
// 3. reset() clears all events
// ---------------------------------------------------------------------------
void testMockReset() {
    MockMidiOutput mock;
    mock.sendNoteOn(60, 90, 1);
    mock.sendNoteOff(60, 0, 1);
    mock.reset();
    ASSERT_EQ(0, mock.eventCount());
    ASSERT_FALSE(mock.hasNoteOn(60));
    ASSERT_FALSE(mock.hasNoteOff(60));
}

// ---------------------------------------------------------------------------
// 4. silence() sends NoteOff only when sounding
// ---------------------------------------------------------------------------
void testSilenceWhenSounding() {
    MockMidiOutput mock;
    LivePlayer p;
    p.midi  = &mock;
    p.note  = 60;
    p.sounding = true;

    p.silence();

    ASSERT_TRUE(mock.hasNoteOff(60));
    ASSERT_EQ(1, mock.noteOffCount());
    ASSERT_FALSE(p.sounding);
}

void testSilenceWhenNotSounding() {
    MockMidiOutput mock;
    LivePlayer p;
    p.midi     = &mock;
    p.note     = 60;
    p.sounding = false;

    p.silence();

    ASSERT_EQ(0, mock.eventCount());
}

// ---------------------------------------------------------------------------
// 5. retrigger() sends NoteOn BEFORE NoteOff (legato contract)
// ---------------------------------------------------------------------------
void testLegatoNoteOnBeforeNoteOff() {
    MockMidiOutput mock;
    LivePlayer p;
    p.midi     = &mock;
    p.note     = 60;
    p.sounding = true;

    p.retrigger(64);  // C4 -> E4

    ASSERT_TRUE(mock.hasNoteOn(64));
    ASSERT_TRUE(mock.hasNoteOff(60));
    // New note-on must appear before old note-off
    ASSERT_TRUE(mock.noteOnBeforeNoteOff(64, 60));
}

// ---------------------------------------------------------------------------
// 6. retrigger() same note: no NoteOff for previous note
// ---------------------------------------------------------------------------
void testRetriggerSameNoteNoOff() {
    MockMidiOutput mock;
    LivePlayer p;
    p.midi     = &mock;
    p.note     = 60;
    p.sounding = true;

    p.retrigger(60);  // same note

    ASSERT_TRUE(mock.hasNoteOn(60));
    ASSERT_FALSE(mock.hasNoteOff(60));  // no NoteOff for same-note retrigger
}

// ---------------------------------------------------------------------------
// 7. retrigger() from silence: sends NoteOn, no old NoteOff
// ---------------------------------------------------------------------------
void testRetriggerFromSilence() {
    MockMidiOutput mock;
    LivePlayer p;
    p.midi     = &mock;
    p.sounding = false;

    p.retrigger(60);

    ASSERT_EQ(1, mock.noteOnCount());
    ASSERT_EQ(0, mock.noteOffCount());
}

// ---------------------------------------------------------------------------
// 8. roll() sends NoteOff then NoteOn (hard re-attack)
// ---------------------------------------------------------------------------
void testRollSendsOffThenOn() {
    MockMidiOutput mock;
    LivePlayer p;
    p.midi     = &mock;
    p.note     = 60;
    p.sounding = true;

    p.roll();

    ASSERT_TRUE(mock.hasNoteOff(60));
    ASSERT_TRUE(mock.hasNoteOn(60));
    // Roll: NoteOff comes BEFORE NoteOn
    ASSERT_TRUE(mock.noteOffBeforeNoteOn(60, 60));
}

// ---------------------------------------------------------------------------
// 9. null guard: no crash when midi is nullptr
// ---------------------------------------------------------------------------
void testNullGuardNoCrash() {
    LivePlayer p;
    p.midi     = nullptr;
    p.note     = 60;
    p.sounding = true;

    p.silence();      // must not crash
    p.retrigger(64);  // must not crash
    p.roll();         // must not crash
    ASSERT_TRUE(true);
}

// ---------------------------------------------------------------------------
// 10. isReady always returns true for mock
// ---------------------------------------------------------------------------
void testMockIsReady() {
    MockMidiOutput mock;
    ASSERT_TRUE(mock.isReady());
}

// ---------------------------------------------------------------------------
// Runner
// ---------------------------------------------------------------------------
void runAll() {
    Serial.println("\n--- Suite: MidiOutput");
    testMockRecordsNoteOn();
    testMockRecordsNoteOff();
    testMockReset();
    testSilenceWhenSounding();
    testSilenceWhenNotSounding();
    testLegatoNoteOnBeforeNoteOff();
    testRetriggerSameNoteNoOff();
    testRetriggerFromSilence();
    testRollSendsOffThenOn();
    testNullGuardNoCrash();
    testMockIsReady();
    TestFramework::printSummary("MidiOutput");
}

} // namespace TestMidiOutput
