#pragma once

#include "RTPEventNotePlus.h"

/**
 * @brief Interface for note playing and management
 * 
 * This interface abstracts note playing operations, allowing for different
 * implementations and enabling proper testing.
 */
class INotesPlayer {
public:
    virtual ~INotesPlayer() = default;

    /**
     * @brief Queue a note for playback
     * @param note Note to queue
     */
    virtual void queueNote(const RTPEventNotePlus& note) = 0;

    /**
     * @brief Play all queued notes
     */
    virtual void playNotes() = 0;

    /**
     * @brief Decrease time-to-live for all playing notes
     */
    virtual void decreaseTimeToLive() = 0;

    /**
     * @brief Stop a specific note
     * @param note Note to stop
     * @return true if note was found and stopped
     */
    virtual bool stopNote(const RTPEventNotePlus& note) = 0;

    /**
     * @brief Stop all notes immediately
     */
    virtual void stopAllNotes() = 0;

    /**
     * @brief Check if a note is currently playing
     * @param note Note to check
     * @return true if note is playing
     */
    virtual bool isNotePlaying(const RTPEventNotePlus& note) const = 0;

    /**
     * @brief Get number of currently playing notes
     * @return Number of playing notes
     */
    virtual size_t getPlayingNoteCount() const = 0;

    /**
     * @brief Get number of queued notes
     * @return Number of queued notes
     */
    virtual size_t getQueuedNoteCount() const = 0;

    /**
     * @brief Clear all queued notes without playing them
     */
    virtual void clearQueue() = 0;
};
