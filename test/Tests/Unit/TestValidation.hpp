#pragma once

#include "Framework/Assert.hpp"
#include "Validation/MidiValidator.hpp"
#include "Validation/InputValidator.hpp"
#include "Validation/RangeChecker.hpp"

namespace TestValidation {

    void testMidiNoteRange() {
        ASSERT_TRUE(MidiValidator::isValidNote(0));
        ASSERT_TRUE(MidiValidator::isValidNote(60));
        ASSERT_TRUE(MidiValidator::isValidNote(127));
        ASSERT_FALSE(MidiValidator::isValidNote(128));
    }

    void testMidiVelocityRange() {
        ASSERT_TRUE(MidiValidator::isValidVelocity(0));
        ASSERT_TRUE(MidiValidator::isValidVelocity(90));
        ASSERT_TRUE(MidiValidator::isValidVelocity(127));
        ASSERT_FALSE(MidiValidator::isValidVelocity(128));
    }

    void testMidiChannelRange() {
        ASSERT_TRUE(MidiValidator::isValidChannel(1));
        ASSERT_TRUE(MidiValidator::isValidChannel(16));
        ASSERT_FALSE(MidiValidator::isValidChannel(0));
        ASSERT_FALSE(MidiValidator::isValidChannel(17));
    }

    void testMidiControlValue() {
        ASSERT_TRUE(MidiValidator::isValidControlValue(0));
        ASSERT_TRUE(MidiValidator::isValidControlValue(127));
        ASSERT_FALSE(MidiValidator::isValidControlValue(128));
    }

    void testBPMRange() {
        ASSERT_TRUE(InputValidator::isValidBPM(40.0f));
        ASSERT_TRUE(InputValidator::isValidBPM(120.0f));
        ASSERT_TRUE(InputValidator::isValidBPM(240.0f));
        ASSERT_FALSE(InputValidator::isValidBPM(39.0f));
        ASSERT_FALSE(InputValidator::isValidBPM(241.0f));
    }

    void testSwingRange() {
        ASSERT_TRUE(InputValidator::isValidSwing(0));
        ASSERT_TRUE(InputValidator::isValidSwing(50));
        ASSERT_TRUE(InputValidator::isValidSwing(100));
        ASSERT_FALSE(InputValidator::isValidSwing(-1));
        ASSERT_FALSE(InputValidator::isValidSwing(101));
    }

    void testQuantizeStrength() {
        ASSERT_TRUE(InputValidator::isValidQuantizeStrength(0));
        ASSERT_TRUE(InputValidator::isValidQuantizeStrength(100));
        ASSERT_FALSE(InputValidator::isValidQuantizeStrength(-1));
        ASSERT_FALSE(InputValidator::isValidQuantizeStrength(101));
    }

    void testRangeCheckerClamp() {
        ASSERT_EQ(RangeChecker::clamp(5, 0, 10), 5);
        ASSERT_EQ(RangeChecker::clamp(-5, 0, 10), 0);
        ASSERT_EQ(RangeChecker::clamp(15, 0, 10), 10);
    }

    void testSceneIndex() {
        ASSERT_TRUE(InputValidator::isValidSceneIndex(0));
        ASSERT_TRUE(InputValidator::isValidSceneIndex(2));
        ASSERT_FALSE(InputValidator::isValidSceneIndex(3));
    }

    void runAll() {
        TEST_SUITE(Validation);
        RUN_TEST(testMidiNoteRange);
        RUN_TEST(testMidiVelocityRange);
        RUN_TEST(testMidiChannelRange);
        RUN_TEST(testMidiControlValue);
        RUN_TEST(testBPMRange);
        RUN_TEST(testSwingRange);
        RUN_TEST(testQuantizeStrength);
        RUN_TEST(testRangeCheckerClamp);
        RUN_TEST(testSceneIndex);
        TestFramework::printSummary("Validation");
    }
}
