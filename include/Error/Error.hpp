#pragma once

#include <cstdint>
#include <stddef.h>
#include <Arduino.h>
#include <WString.h>

/**
 * @brief Error codes for the RTPBuit system
 */
enum class ErrorCode : uint16_t {
    // No error
    None = 0,

    // Hardware Errors (1000-1999)
    HardwareInitFailed = 1000,
    SensorNotFound = 1001,
    DisplayInitFailed = 1002,
    MidiInitFailed = 1003,
    ButtonMatrixInitFailed = 1004,
    RotaryEncoderInitFailed = 1005,

    // Input Errors (2000-2999)
    InvalidNote = 2000,
    InvalidVelocity = 2001,
    InvalidChannel = 2002,
    InvalidController = 2003,
    InvalidButton = 2004,
    InvalidSensor = 2005,
    InvalidState = 2006,

    // Sequencer Errors (3000-3999)
    SequenceNotFound = 3000,
    SceneNotFound = 3001,
    InvalidPosition = 3002,
    SequenceFull = 3003,
    RecordingFailed = 3004,
    PlaybackFailed = 3005,

    // Memory Errors (4000-4999)
    OutOfMemory = 4000,
    InvalidPointer = 4001,
    BufferOverflow = 4002,
    MemoryLeak = 4003,

    // File System Errors (5000-5999)
    FileNotFound = 5000,
    FileCorrupted = 5001,
    FileSystemError = 5002,
    PermissionDenied = 5003,
    DiskFull = 5004,

    // Configuration Errors (6000-6999)
    InvalidConfig = 6000,
    ConfigNotFound = 6001,
    ConfigCorrupted = 6002,
    InvalidParameter = 6003,
    ServiceNotFound = 6004,

    // State Machine Errors (7000-7999)
    InvalidStateTransition = 7000,
    StateNotFound = 7001,
    StateMachineError = 7002,

    // MIDI Errors (8000-8999)
    MidiMessageTooLong = 8000,
    InvalidMidiMessage = 8001,
    MidiBufferFull = 8002,
    MidiTimeout = 8003,

    // Timing Errors (9000-9999)
    ClockError = 9000,
    TimeoutError = 9001,
    TimingViolation = 9002,

    // Unknown Error
    Unknown = 9999
};

/**
 * @brief Error severity levels
 */
enum class ErrorSeverity : uint8_t {
    Debug = 0,      // Debug information only
    Info = 1,       // Informational message
    Warning = 2,    // Warning that might need attention
    Error = 3,      // Error that prevents normal operation
    Critical = 4    // Critical error that requires immediate attention
};

/**
 * @brief Error information structure
 */
struct Error {
    ErrorCode code;
    ErrorSeverity severity;
    String message;
    String context;
    uint32_t timestamp;
    uint16_t line;
    const char* file;

    Error(ErrorCode c, ErrorSeverity s, const String& msg, const String& ctx = "", 
          uint16_t l = 0, const char* f = nullptr)
        : code(c), severity(s), message(msg), context(ctx), 
          timestamp(millis()), line(l), file(f) {}

    String toString() const;
    bool isNone() const { return code == ErrorCode::None; }
    bool isError() const { return severity >= ErrorSeverity::Error; }
    bool isWarning() const { return severity == ErrorSeverity::Warning; }
    bool isCritical() const { return severity >= ErrorSeverity::Critical; }
};

/**
 * @brief Error utilities
 */
namespace ErrorUtils {
    String getErrorCodeString(ErrorCode code);
    String getSeverityString(ErrorSeverity severity);
    ErrorCode fromMidiError(uint8_t midiError);
    bool isRecoverable(ErrorCode code);
    bool requiresImmediateAction(ErrorCode code);
}
