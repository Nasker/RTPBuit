#pragma once

#include <cstdint>
#include <stddef.h>
#include <Arduino.h>
#include <WString.h>
#include "Structs.h"

// Forward declarations
class NotesPlayer;
class MusicManager;

/**
 * @brief Interface for sequencer operations
 * 
 * This interface abstracts sequencer functionality, allowing for different
 * sequencer implementations and enabling proper testing.
 */
class ISequencer {
public:
    virtual ~ISequencer() = default;

    // Playback Control
    virtual void play() = 0;
    virtual void stop() = 0;
    virtual void pause() = 0;
    virtual bool isPlaying() const = 0;

    // Scene Management
    virtual void selectScene(uint8_t scene) = 0;
    virtual void nextScene() = 0;
    virtual void previousScene() = 0;
    virtual uint8_t getCurrentScene() const = 0;
    virtual uint8_t getSceneCount() const = 0;
    virtual void addDynamicScene() = 0;
    virtual void removeCurrentScene() = 0;
    virtual int getNumScenes() const = 0;

    // Sequence Management
    virtual void selectSequence(uint8_t sequence) = 0;
    virtual uint8_t getCurrentSequence() const = 0;
    virtual void toggleSequence(uint8_t sequence) = 0;
    virtual void toggleAllSequences() = 0;

    // Note Editing
    virtual void toggleNote(uint16_t position) = 0;
    virtual void editNote(uint16_t position, bool state) = 0;
    virtual void editNote(uint16_t position, uint8_t note, uint8_t velocity) = 0;

    // Live Playing
    virtual void playLiveNoteOn(uint8_t rootNote, uint8_t velocity, uint8_t chordType) = 0;
    virtual void playLiveNoteOff(uint8_t rootNote, uint8_t chordType) = 0;
    virtual void handleLiveThreeAxis(int left, int center, int right) = 0;
    virtual void handleLiveTick() = 0;
    virtual void handleLiveFineTick() = 0;
    virtual uint8_t getLiveVelocity() = 0;

    // Recording
    virtual bool isRecording() const = 0;
    virtual void toggleRecording() = 0;

    // State Queries
    virtual uint16_t getCurrentPosition() const = 0;
    virtual uint16_t getSequenceLength() const = 0;
    virtual uint8_t getSequenceType() const = 0;
    virtual uint8_t getMidiChannel() const = 0;
    virtual String getSequenceTypeName() const = 0;
    virtual uint32_t getSequenceColor() const = 0;
    virtual RTPSequenceNoteStates getNoteStates() const = 0;
    virtual RTPSequencesState getSequencesState() const = 0;

    // Parameter Control
    virtual void selectParameter(uint8_t parameter) = 0;
    virtual void increaseParameter() = 0;
    virtual void decreaseParameter() = 0;
    virtual int getParameterValue() const = 0;
    virtual String getParameterName() const = 0;

    // Navigation
    virtual void nextPage() = 0;
    virtual void previousPage() = 0;
    virtual uint8_t getCurrentPage() const = 0;
    virtual uint16_t getPageOffset() const = 0;

    // Persistence
    virtual void saveToStorage(const String& filename) = 0;
    virtual void loadFromStorage(const String& filename) = 0;
    virtual bool fileExists(const String& filename) const = 0;
};
