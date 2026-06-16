/**
 * @file test_main.cpp
 * @brief Test runner entry point for Teensy 4.1
 * 
 * Compile with the [env:teensy41_test] environment.
 * Results are reported over Serial at 115200 baud.
 */

#include <Arduino.h>
#include "Framework/Assert.hpp"
#include "Tests/Unit/TestValidation.hpp"

void setup() {
    Serial.begin(115200);
    delay(2000);  // Wait for Serial monitor

    Serial.println("\n========== RTPBuit Unit Tests ==========");

    TestValidation::runAll();

    Serial.println("\n========== Tests Complete ==========");
}

void loop() {
    // Nothing — tests run once in setup
}
