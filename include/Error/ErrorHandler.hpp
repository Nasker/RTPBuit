#pragma once

#include "Error.hpp"
#include "Result.hpp"
#include "Logger.hpp"
#include <functional>
#include <vector>

/**
 * @brief Error handling policies
 */
enum class ErrorPolicy {
    LogOnly,           // Just log the error
    LogAndRecover,     // Log and attempt recovery
    LogAndHalt,        // Log and stop the system
    LogAndRestart,     // Log and restart the system
    Custom             // Use custom handler
};

/**
 * @brief Error recovery strategies
 */
enum class RecoveryStrategy {
    None,              // No recovery attempt
    Retry,             // Retry the operation
    Fallback,          // Use fallback mechanism
    ResetComponent,    // Reset the affected component
    SafeMode           // Enter safe mode
};

/**
 * @brief Error handler for centralized error management
 * 
 * Provides centralized error handling with configurable policies and recovery strategies.
 */
class ErrorHandler {
private:
    static ErrorPolicy _defaultPolicy;
    static std::vector<std::function<void(const Error&)>> _errorCallbacks;
    static uint32_t _errorCount;
    static uint32_t _lastErrorTime;
    static Error _lastError;

public:
    // Configuration
    static void setDefaultPolicy(ErrorPolicy policy) { _defaultPolicy = policy; }
    static void addErrorCallback(std::function<void(const Error&)> callback);
    static void clearErrorCallbacks();

    // Error handling
    static void handleError(const Error& error);
    static void handleError(ErrorCode code, ErrorSeverity severity, const String& message, const String& context = "");
    
    // Error recovery
    static bool attemptRecovery(const Error& error, RecoveryStrategy strategy);
    static bool canRecover(ErrorCode code);
    static RecoveryStrategy getRecoveryStrategy(ErrorCode code);

    // Error statistics
    static uint32_t getErrorCount() { return _errorCount; }
    static uint32_t getTimeSinceLastError() { return millis() - _lastErrorTime; }
    static const Error& getLastError() { return _lastError; }
    static void resetStatistics();

    // System state
    static bool isSystemHealthy();
    static void enterSafeMode();
    static void emergencyStop();

    // Specific error handlers
    static void handleHardwareError(const Error& error);
    static void handleMidiError(const Error& error);
    static void handleSequencerError(const Error& error);
    static void handleMemoryError(const Error& error);
    static void handleFileSystemError(const Error& error);

    // Error reporting
    static String generateErrorReport();
    static void printErrorSummary();

private:
    static void executeCallbacks(const Error& error);
    static void logError(const Error& error);
    static void applyPolicy(const Error& error, ErrorPolicy policy);
    static bool retryOperation(std::function<bool()> operation, uint8_t maxRetries = 3);
};

/**
 * @brief RAII error guard for automatic error handling
 * 
 * Automatically handles errors in a scope and ensures cleanup.
 */
class ErrorGuard {
private:
    String _scope;
    bool _active;

public:
    explicit ErrorGuard(const String& scope) : _scope(scope), _active(true) {
        LOG_DEBUG_CTX("Entering scope: " + _scope, "ErrorGuard");
    }

    ~ErrorGuard() {
        if (_active) {
            LOG_DEBUG_CTX("Exiting scope: " + _scope, "ErrorGuard");
        }
    }

    void dismiss() { _active = false; }
    void reportError(const Error& error) {
        ErrorHandler::handleError(error);
    }
};

// Convenience macros for error handling
#define HANDLE_ERROR(error) ErrorHandler::handleError(error)
#define HANDLE_ERROR_CODE(code, severity, msg) ErrorHandler::handleError(code, severity, msg, __FUNCTION__)
#define ERROR_GUARD(name) ErrorGuard _guard(name)

// Error checking macros
#define CHECK_RESULT(result) \
    do { \
        auto _check_result = (result); \
        if (_check_result.isError()) { \
            ErrorHandler::handleError(_check_result.getError()); \
        } \
    } while(0)

#define CHECK_RESULT_RETURN(result) \
    do { \
        auto _check_result = (result); \
        if (_check_result.isError()) { \
            ErrorHandler::handleError(_check_result.getError()); \
            return _check_result.getError(); \
        } \
    } while(0)

#define CHECK_BOOL(condition, error_code, message) \
    do { \
        if (!(condition)) { \
            ErrorHandler::handleError(error_code, ErrorSeverity::Error, message, __FUNCTION__); \
            return false; \
        } \
    } while(0)

#define CHECK_BOOL_RETURN(condition, error_code, message) \
    do { \
        if (!(condition)) { \
            ErrorHandler::handleError(error_code, ErrorSeverity::Error, message, __FUNCTION__); \
            return; \
        } \
    } while(0)
