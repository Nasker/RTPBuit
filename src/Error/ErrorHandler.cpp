#include "Error/ErrorHandler.hpp"
#include <Arduino.h>

// Static member initialization
ErrorPolicy ErrorHandler::_defaultPolicy = ErrorPolicy::LogOnly;
std::vector<std::function<void(const Error&)>> ErrorHandler::_errorCallbacks;
uint32_t ErrorHandler::_errorCount = 0;
uint32_t ErrorHandler::_lastErrorTime = 0;
Error ErrorHandler::_lastError = Error(ErrorCode::None, ErrorSeverity::Info, "", "");

void ErrorHandler::handleError(ErrorCode code, ErrorSeverity severity, 
                               const String& message, const String& context) {
    // Create error object
    Error error(code, severity, message, context);
    handleError(error);
}

void ErrorHandler::handleError(const Error& error) {
    // Update statistics
    _errorCount++;
    _lastErrorTime = millis();
    _lastError = error;
    
    // Log the error
    String severityStr;
    switch (error.severity) {
        case ErrorSeverity::Info:    severityStr = "INFO"; break;
        case ErrorSeverity::Warning: severityStr = "WARN"; break;
        case ErrorSeverity::Error:   severityStr = "ERROR"; break;
        case ErrorSeverity::Critical: severityStr = "CRITICAL"; break;
        default: severityStr = "UNKNOWN"; break;
    }
    
    String errorMsg = "[" + severityStr + "] " + error.context + ": " + 
                     error.message + " (Code: " + String(static_cast<int>(error.code)) + ")";
    
    Serial.println(errorMsg);
    
    // Execute callbacks
    executeCallbacks(error);
    
    // Apply policy
    applyPolicy(error, _defaultPolicy);
}

void ErrorHandler::executeCallbacks(const Error& error) {
    for (auto& callback : _errorCallbacks) {
        if (callback) callback(error);
    }
}

void ErrorHandler::applyPolicy(const Error& error, ErrorPolicy policy) {
    switch (policy) {
        case ErrorPolicy::LogOnly:
            // Already logged
            break;
        case ErrorPolicy::LogAndHalt:
            if (error.severity == ErrorSeverity::Critical) {
                Serial.println("CRITICAL ERROR - System halted");
                while(1); // Halt
            }
            break;
        case ErrorPolicy::LogAndRecover:
            // Attempt recovery (stub for now)
            break;
        case ErrorPolicy::LogAndRestart:
            if (error.severity == ErrorSeverity::Critical) {
                // Could trigger watchdog reset
            }
            break;
        case ErrorPolicy::Custom:
            // Custom handlers would be called via callbacks
            break;
    }
}

void ErrorHandler::addErrorCallback(std::function<void(const Error&)> callback) {
    _errorCallbacks.push_back(callback);
}

void ErrorHandler::clearErrorCallbacks() {
    _errorCallbacks.clear();
}

void ErrorHandler::resetStatistics() {
    _errorCount = 0;
    _lastErrorTime = 0;
}

bool ErrorHandler::isSystemHealthy() {
    // System is healthy if no critical errors in last 5 seconds
    return (_lastError.severity != ErrorSeverity::Critical) || 
           (millis() - _lastErrorTime > 5000);
}

// Stub implementations for other methods
bool ErrorHandler::attemptRecovery(const Error& error, RecoveryStrategy strategy) { return false; }
bool ErrorHandler::canRecover(ErrorCode code) { return false; }
RecoveryStrategy ErrorHandler::getRecoveryStrategy(ErrorCode code) { return RecoveryStrategy::None; }
void ErrorHandler::enterSafeMode() { Serial.println("Entering safe mode"); }
void ErrorHandler::emergencyStop() { Serial.println("Emergency stop"); while(1); }
void ErrorHandler::handleHardwareError(const Error& error) { handleError(error); }
void ErrorHandler::handleMidiError(const Error& error) { handleError(error); }
void ErrorHandler::handleSequencerError(const Error& error) { handleError(error); }
void ErrorHandler::handleMemoryError(const Error& error) { handleError(error); }
void ErrorHandler::handleFileSystemError(const Error& error) { handleError(error); }
String ErrorHandler::generateErrorReport() { return "Error report not implemented"; }
void ErrorHandler::printErrorSummary() { Serial.println("Error summary not implemented"); }
bool ErrorHandler::retryOperation(std::function<bool()> operation, uint8_t maxRetries) { return false; }
void ErrorHandler::logError(const Error& error) { Serial.println(error.message); }
