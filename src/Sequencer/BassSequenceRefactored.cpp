/**
 * @file BassSequenceRefactored.cpp
 * @brief Example of refactored BassSequence using new architecture
 * 
 * This demonstrates how to migrate the existing BassSequence to use
 * the new interfaces, dependency injection, and error handling.
 */

#include "StateMachine/BassSequence.hpp"
#include "Interfaces/IMidiOutput.hpp"
#include "Error/Result.hpp"
#include "Error/ErrorHandler.hpp"
#include "Config/MusicConfig.hpp"
#include "Config/HardwareConfig.hpp"
#include "Validation/MidiValidator.hpp"
#include "Hardware/Implementations/TeensyMidiOutput.hpp"
#include <memory>

// Refactored BassSequence showing the new approach
class BassSequenceRefactored {
private:
    std::shared_ptr<IMidiOutput> _midiOutput;
    uint8_t _midiChannel;
    bool _chordLatched;
    uint8_t _currentRootNote;
    uint8_t _currentChordType;
    bool _sounding;
    uint8_t _currentNote;

public:
    // Constructor with dependency injection
    BassSequenceRefactored(std::shared_ptr<IMidiOutput> midiOutput, uint8_t midiChannel)
        : _midiOutput(midiOutput), _midiChannel(midiChannel), 
          _chordLatched(false), _currentRootNote(0), _currentChordType(0),
          _sounding(false), _currentNote(0) {
        
        if (!_midiOutput) {
            ErrorHandler::handleError(ErrorCode::InvalidState, ErrorSeverity::Critical,
                                     "MIDI output cannot be null", "BassSequenceRefactored");
        }
    }

    // Refactored method with error handling
    Result<void> playLiveNoteOn(uint8_t rootNote, uint8_t velocity, uint8_t chordType) {
        // Validate inputs using configuration constants
        if (!MidiValidator::isValidNote(rootNote)) {
            return Result<void>::failure(ErrorCode::InvalidNote, ErrorSeverity::Error,
                                      "Note out of range: " + String(rootNote));
        }
        
        if (!MidiValidator::isValidVelocity(velocity)) {
            return Result<void>::failure(ErrorCode::InvalidVelocity, ErrorSeverity::Error,
                                      "Velocity out of range: " + String(velocity));
        }
        
        if (!MidiValidator::isValidChannel(_midiChannel)) {
            return Result<void>::failure(ErrorCode::InvalidChannel, ErrorSeverity::Error,
                                      "Invalid MIDI channel: " + String(_midiChannel));
        }

        // Latch the chord
        _chordLatched = true;
        _currentRootNote = rootNote;
        _currentChordType = chordType;

        // Play the note using the injected interface
        if (!_midiOutput) {
            return Result<void>::failure(ErrorCode::InvalidPointer, ErrorSeverity::Error,
                                      "MIDI output not available");
        }
        
        _midiOutput->sendNoteOn(rootNote, velocity, _midiChannel);
        _currentNote = rootNote;
        _sounding = true;
        
        LOG_DEBUG_CTX("Note played: " + String(rootNote) + " vel: " + String(velocity), 
                     "BassSequenceRefactored");
        
        return Result<void>::Ok();
    }

    Result<void> playLiveNoteOff(uint8_t rootNote, uint8_t chordType) {
        if (!_midiOutput) {
            return Result<void>::failure(ErrorCode::InvalidState, ErrorSeverity::Error,
                                      "MIDI output not available");
        }

        if (!_sounding || _currentNote != rootNote) {
            return Result<void>::Ok(); // Note not playing, nothing to do
        }

        _midiOutput->sendNoteOff(rootNote, 0, _midiChannel);
        _sounding = false;
        
        LOG_DEBUG_CTX("Note stopped: " + String(rootNote), "BassSequenceRefactored");
        
        return Result<void>::Ok();
    }

    // Method showing the old legato implementation with new error handling
    Result<void> retriggerLiveNote(uint8_t targetNote, uint8_t velocity) {
        if (!_chordLatched) {
            return Result<void>::failure(ErrorCode::InvalidState, ErrorSeverity::Warning,
                                      "No chord latched for retrigger");
        }

        if (!_midiOutput) {
            return Result<void>::failure(ErrorCode::InvalidState, ErrorSeverity::Error,
                                      "MIDI output not available");
        }

        // Validate target note
        if (!MidiValidator::isValidNote(targetNote)) {
            return Result<void>::failure(ErrorCode::InvalidNote, ErrorSeverity::Error,
                                      "Target note out of range: " + String(targetNote));
        }

        // Legato: start new note before stopping old one
        uint8_t oldNote = _currentNote;
        bool wasSounding = _sounding;

        // Start new note first (legato)
        _midiOutput->sendNoteOn(targetNote, velocity, _midiChannel);
        _currentNote = targetNote;
        _sounding = true;

        // Then stop old note if it was different
        if (wasSounding && oldNote != targetNote) {
            _midiOutput->sendNoteOff(oldNote, 0, _midiChannel);
        }

        LOG_DEBUG_CTX("Legato retrigger: " + String(oldNote) + " -> " + String(targetNote),
                     "BassSequenceRefactored");
        
        return Result<void>::Ok();
    }

    // Getters
    bool isChordLatched() const { return _chordLatched; }
    uint8_t getCurrentRootNote() const { return _currentRootNote; }
    uint8_t getCurrentChordType() const { return _currentChordType; }
    bool isSounding() const { return _sounding; }
    uint8_t getCurrentNote() const { return _currentNote; }
    uint8_t getMidiChannel() const { return _midiChannel; }

    // Utility method to release latch
    void releaseChord() {
        _chordLatched = false;
        if (_sounding) {
            _midiOutput->sendNoteOff(_currentNote, 0, _midiChannel);
            _sounding = false;
        }
        LOG_DEBUG_CTX("Chord latch released", "BassSequenceRefactored");
    }
};

// Example of how to integrate with the existing system
class BassSequenceFactory {
public:
    static std::unique_ptr<BassSequenceRefactored> create(std::shared_ptr<IMidiOutput> midiOutput, 
                                                         uint8_t midiChannel = 1) {
        if (!midiOutput) {
            ErrorHandler::handleError(ErrorCode::InvalidPointer, ErrorSeverity::Critical,
                                     "Cannot create BassSequence with null MIDI output",
                                     "BassSequenceFactory");
            return nullptr;
        }

        return std::make_unique<BassSequenceRefactored>(midiOutput, midiChannel);
    }
};

// Usage example in the context of the larger system
void demonstrateRefactoredUsage() {
    // Directly instantiate dependencies (ServiceContainer requires RTTI which is disabled on Teensy)
    auto midiOutput = std::make_shared<TeensyMidiOutput>();
    if (!midiOutput) {
        ErrorHandler::handleError(ErrorCode::InvalidPointer, ErrorSeverity::Critical,
                                 "Failed to create MIDI output", "demonstrateUsage");
        return;
    }

    // Create the refactored bass sequence
    auto bassSeq = BassSequenceFactory::create(midiOutput, 1);
    if (!bassSeq) {
        LOG_ERROR_CTX("Failed to create BassSequence", "demonstrateUsage");
        return;
    }

    // Use the refactored interface with error handling
    auto result = bassSeq->playLiveNoteOn(60, 90, 0); // C4, velocity 90, chord type 0
    if (result.isError()) {
        ErrorHandler::handleError(result.getError());
        return;
    }

    // Retrigger with legato
    auto retriggerResult = bassSeq->retriggerLiveNote(64, 90); // E4
    if (retriggerResult.isError()) {
        ErrorHandler::handleError(retriggerResult.getError());
    }

    // Clean up
    bassSeq->releaseChord();
    LOG_INFO_CTX("Refactored BassSequence demo completed", "demonstrateUsage");
}
