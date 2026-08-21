#pragma once

#include <Arduino.h>

/**
 * @brief Lightweight assertion macros for embedded unit testing
 * 
 * Outputs results over Serial. Include in test files only.
 */

namespace TestFramework {
    inline uint16_t& _passCount() { static uint16_t c = 0; return c; }
    inline uint16_t& _failCount() { static uint16_t c = 0; return c; }

    inline void recordPass() { _passCount()++; }
    inline void recordFail(const char* file, int line, const char* expr) {
        _failCount()++;
        Serial.print("  FAIL [");
        Serial.print(file);
        Serial.print(":");
        Serial.print(line);
        Serial.print("] ");
        Serial.println(expr);
    }
    inline void printSummary(const char* suiteName) {
        Serial.print("\n=== ");
        Serial.print(suiteName);
        Serial.print(" | PASS: ");
        Serial.print(_passCount());
        Serial.print("  FAIL: ");
        Serial.println(_failCount());
        _passCount() = 0;
        _failCount() = 0;
    }
}

#define ASSERT_TRUE(expr) \
    do { \
        if (expr) { TestFramework::recordPass(); } \
        else { TestFramework::recordFail(__FILE__, __LINE__, #expr " expected true"); } \
    } while(0)

#define ASSERT_FALSE(expr) \
    do { \
        if (!(expr)) { TestFramework::recordPass(); } \
        else { TestFramework::recordFail(__FILE__, __LINE__, #expr " expected false"); } \
    } while(0)

#define ASSERT_EQ(a, b) \
    do { \
        if ((a) == (b)) { TestFramework::recordPass(); } \
        else { TestFramework::recordFail(__FILE__, __LINE__, #a " == " #b); } \
    } while(0)

#define ASSERT_NE(a, b) \
    do { \
        if ((a) != (b)) { TestFramework::recordPass(); } \
        else { TestFramework::recordFail(__FILE__, __LINE__, #a " != " #b); } \
    } while(0)

#define ASSERT_NULL(ptr) \
    do { \
        if ((ptr) == nullptr) { TestFramework::recordPass(); } \
        else { TestFramework::recordFail(__FILE__, __LINE__, #ptr " expected nullptr"); } \
    } while(0)

#define ASSERT_NOT_NULL(ptr) \
    do { \
        if ((ptr) != nullptr) { TestFramework::recordPass(); } \
        else { TestFramework::recordFail(__FILE__, __LINE__, #ptr " expected non-null"); } \
    } while(0)

#define RUN_TEST(name) \
    do { \
        Serial.print("  TEST: "); Serial.println(#name); \
        name(); \
    } while(0)

#define TEST_SUITE(name) \
    do { \
        Serial.print("\n--- Suite: "); Serial.println(#name); \
    } while(0)
