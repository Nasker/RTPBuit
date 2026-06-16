#pragma once

#include "Error.hpp"
#include <cstdint>
#include <WString.h>

/**
 * @brief Log levels for the logging system
 */
enum class LogLevel : uint8_t {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

/**
 * @brief Logger interface for embedded systems
 * 
 * Provides a simple, configurable logging system that can be disabled
 * in production builds to save memory and CPU cycles.
 */
class Logger {
public:
    static inline void debug(const String& message, const String& context = "") {
        Serial.print("[DBG]"); if (context.length()) { Serial.print("["); Serial.print(context); Serial.print("] "); } Serial.println(message);
    }
    static inline void info(const String& message, const String& context = "") {
        Serial.print("[INF]"); if (context.length()) { Serial.print("["); Serial.print(context); Serial.print("] "); } Serial.println(message);
    }
    static inline void warning(const String& message, const String& context = "") {
        Serial.print("[WRN]"); if (context.length()) { Serial.print("["); Serial.print(context); Serial.print("] "); } Serial.println(message);
    }
    static inline void error(const String& message, const String& context = "") {
        Serial.print("[ERR]"); if (context.length()) { Serial.print("["); Serial.print(context); Serial.print("] "); } Serial.println(message);
    }
    static inline void critical(const String& message, const String& context = "") {
        Serial.print("[CRT]"); if (context.length()) { Serial.print("["); Serial.print(context); Serial.print("] "); } Serial.println(message);
    }
    static inline void logError(const Error& err) {
        error(err.message, err.context);
    }
    static inline void logError(ErrorCode code, ErrorSeverity severity, const String& message, const String& context = "") {
        error(message, context);
    }
    static inline void flush() {}
    static inline String getLevelString(LogLevel level) {
        switch (level) {
            case LogLevel::Debug:    return "DBG";
            case LogLevel::Info:     return "INF";
            case LogLevel::Warning:  return "WRN";
            case LogLevel::Error:    return "ERR";
            case LogLevel::Critical: return "CRT";
            default:                 return "???";
        }
    }
};

// Convenience macros for logging
#define LOG_DEBUG(msg) Logger::debug(msg, __FUNCTION__)
#define LOG_INFO(msg) Logger::info(msg, __FUNCTION__)
#define LOG_WARNING(msg) Logger::warning(msg, __FUNCTION__)
#define LOG_ERROR(msg) Logger::error(msg, __FUNCTION__)
#define LOG_CRITICAL(msg) Logger::critical(msg, __FUNCTION__)

#define LOG_DEBUG_CTX(msg, ctx) Logger::debug(msg, ctx)
#define LOG_INFO_CTX(msg, ctx) Logger::info(msg, ctx)
#define LOG_WARNING_CTX(msg, ctx) Logger::warning(msg, ctx)
#define LOG_ERROR_CTX(msg, ctx) Logger::error(msg, ctx)
#define LOG_CRITICAL_CTX(msg, ctx) Logger::critical(msg, ctx)

#define LOG_ERROR_OBJ(err) Logger::logError(err)

// Performance logging macros
#define LOG_PERFORMANCE_START(name) \
    uint32_t _perf_start_##name = micros()

#define LOG_PERFORMANCE_END(name) \
    do { \
        uint32_t _perf_end_##name = micros(); \
        uint32_t _perf_duration_##name = _perf_end_##name - _perf_start_##name; \
        LOG_DEBUG_CTX(String("Performance: ") + #name + " took " + _perf_duration_##name + " microseconds", "PERFORMANCE"); \
    } while(0)

// Conditional logging for debug builds
#ifdef DEBUG
    #define DLOG_DEBUG(msg) LOG_DEBUG(msg)
    #define DLOG_INFO(msg) LOG_INFO(msg)
    #define DLOG_WARNING(msg) LOG_WARNING(msg)
    #define DLOG_ERROR(msg) LOG_ERROR(msg)
    #define DLOG_CRITICAL(msg) LOG_CRITICAL(msg)
#else
    #define DLOG_DEBUG(msg) do {} while(0)
    #define DLOG_INFO(msg) do {} while(0)
    #define DLOG_WARNING(msg) do {} while(0)
    #define DLOG_ERROR(msg) do {} while(0)
    #define DLOG_CRITICAL(msg) LOG_CRITICAL(msg)  // Always log critical errors
#endif
