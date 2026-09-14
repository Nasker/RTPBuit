#pragma once

#include "Framework/Assert.hpp"
#include "Mocks/MockMidiOutput.hpp"
#include "Midi/MidiRouter.hpp"
#include "Midi/MidiMessage.hpp"

/**
 * Tests for MidiRouter routing table matching, destination dispatch,
 * default routes, and destOverride bypass.
 */
namespace TestMidiRouter {

// ---------------------------------------------------------------------------
// 1. Default-constructed router has no routes and no outputs
// ---------------------------------------------------------------------------
void testEmptyRouterNoRoutes() {
    MidiRouter router;
    ASSERT_EQ(0, router.getRouteCount());
    ASSERT_NULL(router.getOutput(MidiPort::USB_DEVICE));
    ASSERT_NULL(router.getOutput(MidiPort::DIN));
    ASSERT_NULL(router.getOutput(MidiPort::INTERNAL));
}

// ---------------------------------------------------------------------------
// 2. addRoute increments count, removeRoute decrements
// ---------------------------------------------------------------------------
void testAddRemoveRoute() {
    MidiRouter router;
    bool ok = router.addRoute(MidiPort::USB_DEVICE, MidiPort::INTERNAL);
    ASSERT_TRUE(ok);
    ASSERT_EQ(1, router.getRouteCount());

    router.removeRoute(0);
    ASSERT_EQ(0, router.getRouteCount());
}

// ---------------------------------------------------------------------------
// 3. clearRoutes resets to zero
// ---------------------------------------------------------------------------
void testClearRoutes() {
    MidiRouter router;
    router.addRoute(MidiPort::USB_DEVICE, MidiPort::INTERNAL);
    router.addRoute(MidiPort::DIN, MidiPort::INTERNAL);
    ASSERT_EQ(2, router.getRouteCount());

    router.clearRoutes();
    ASSERT_EQ(0, router.getRouteCount());
}

// ---------------------------------------------------------------------------
// 4. Route matching: NoteOn from USB_DEVICE → INTERNAL
// ---------------------------------------------------------------------------
void testRouteNoteOnToInternal() {
    MidiRouter router;
    MockMidiOutput mockInternal;
    router.setOutput(MidiPort::INTERNAL, &mockInternal);
    router.addRoute(MidiPort::USB_DEVICE, MidiPort::INTERNAL, MidiTypeMask::NOTES);

    MidiMessage msg { MidiMessage::NoteOn, 1, 60, 90, MidiPort::USB_DEVICE };
    router.route(msg);

    ASSERT_EQ(1, mockInternal.eventCount());
    ASSERT_TRUE(mockInternal.hasNoteOn(60, 1));
}

// ---------------------------------------------------------------------------
// 5. Message type filter: CC not forwarded when only NOTES mask
// ---------------------------------------------------------------------------
void testTypeFilterBlocksCC() {
    MidiRouter router;
    MockMidiOutput mockInternal;
    router.setOutput(MidiPort::INTERNAL, &mockInternal);
    router.addRoute(MidiPort::USB_DEVICE, MidiPort::INTERNAL, MidiTypeMask::NOTES);

    MidiMessage msg { MidiMessage::ControlChange, 1, 74, 100, MidiPort::USB_DEVICE };
    router.route(msg);

    ASSERT_EQ(0, mockInternal.eventCount());
}

// ---------------------------------------------------------------------------
// 6. Multi-port destination: one message goes to two outputs
// ---------------------------------------------------------------------------
void testMultiPortDestination() {
    MidiRouter router;
    MockMidiOutput mockUsb;
    MockMidiOutput mockDin;
    router.setOutput(MidiPort::USB_DEVICE, &mockUsb);
    router.setOutput(MidiPort::DIN, &mockDin);
    router.addRoute(MidiPort::INTERNAL, MidiPort::USB_DEVICE | MidiPort::DIN, MidiTypeMask::ALL);

    MidiMessage msg { MidiMessage::NoteOn, 3, 48, 80, MidiPort::INTERNAL };
    router.route(msg);

    ASSERT_EQ(1, mockUsb.eventCount());
    ASSERT_EQ(1, mockDin.eventCount());
    ASSERT_TRUE(mockUsb.hasNoteOn(48, 3));
    ASSERT_TRUE(mockDin.hasNoteOn(48, 3));
}

// ---------------------------------------------------------------------------
// 7. Source echo prevention: message not sent back to its own port
// ---------------------------------------------------------------------------
void testNoEchoToSource() {
    MidiRouter router;
    MockMidiOutput mockUsb;
    router.setOutput(MidiPort::USB_DEVICE, &mockUsb);
    // Route USB_DEVICE → USB_DEVICE | INTERNAL (INTERNAL has no output registered)
    router.addRoute(MidiPort::USB_DEVICE, MidiPort::USB_DEVICE | MidiPort::INTERNAL, MidiTypeMask::ALL);

    MidiMessage msg { MidiMessage::NoteOn, 1, 60, 90, MidiPort::USB_DEVICE };
    router.route(msg);

    // Should NOT be sent back to USB_DEVICE (source echo prevention)
    ASSERT_EQ(0, mockUsb.eventCount());
}

// ---------------------------------------------------------------------------
// 8. Channel range filter
// ---------------------------------------------------------------------------
void testChannelRangeFilter() {
    MidiRouter router;
    MockMidiOutput mockInternal;
    router.setOutput(MidiPort::INTERNAL, &mockInternal);
    // Only allow channels 1-4
    router.addRoute(MidiPort::USB_DEVICE, MidiPort::INTERNAL, MidiTypeMask::ALL, 1, 4);

    MidiMessage msg1 { MidiMessage::NoteOn, 3, 60, 90, MidiPort::USB_DEVICE };
    router.route(msg1);
    ASSERT_EQ(1, mockInternal.eventCount());

    mockInternal.reset();
    MidiMessage msg2 { MidiMessage::NoteOn, 10, 60, 90, MidiPort::USB_DEVICE };
    router.route(msg2);
    ASSERT_EQ(0, mockInternal.eventCount());
}

// ---------------------------------------------------------------------------
// 9. Disabled route is skipped
// ---------------------------------------------------------------------------
void testDisabledRouteSkipped() {
    MidiRouter router;
    MockMidiOutput mockInternal;
    router.setOutput(MidiPort::INTERNAL, &mockInternal);
    router.addRoute(MidiPort::USB_DEVICE, MidiPort::INTERNAL, MidiTypeMask::ALL);
    router.enableRoute(0, false);

    MidiMessage msg { MidiMessage::NoteOn, 1, 60, 90, MidiPort::USB_DEVICE };
    router.route(msg);

    ASSERT_EQ(0, mockInternal.eventCount());
}

// ---------------------------------------------------------------------------
// 10. destOverride bypasses routing table
// ---------------------------------------------------------------------------
void testDestOverrideBypassesTable() {
    MidiRouter router;
    MockMidiOutput mockUsb;
    MockMidiOutput mockDin;
    router.setOutput(MidiPort::USB_DEVICE, &mockUsb);
    router.setOutput(MidiPort::DIN, &mockDin);
    // No routes at all — but destOverride should still work
    ASSERT_EQ(0, router.getRouteCount());

    MidiMessage msg { MidiMessage::NoteOn, 5, 72, 100, MidiPort::INTERNAL };
    msg.destOverride = MidiPort::DIN;
    router.route(msg);

    ASSERT_EQ(0, mockUsb.eventCount());
    ASSERT_EQ(1, mockDin.eventCount());
    ASSERT_TRUE(mockDin.hasNoteOn(72, 5));
}

// ---------------------------------------------------------------------------
// 11. RealTime message routing
// ---------------------------------------------------------------------------
void testRealTimeRouting() {
    MidiRouter router;
    MockMidiOutput mockInternal;
    router.setOutput(MidiPort::INTERNAL, &mockInternal);
    router.addRoute(MidiPort::DIN, MidiPort::INTERNAL, MidiTypeMask::REAL_TIME);

    MidiMessage msg { MidiMessage::RealTime, 0, 0xF8, 0, MidiPort::DIN };
    router.route(msg);

    ASSERT_EQ(1, mockInternal.eventCount());
    ASSERT_EQ(MockMidiOutput::MidiEvent::Type::RealTime, mockInternal._events[0].type);
    ASSERT_EQ(0xF8, mockInternal._events[0].byte1);
}

// ---------------------------------------------------------------------------
// 12. setDefaultRoutes creates backward-compatible routing
// ---------------------------------------------------------------------------
void testDefaultRoutes() {
    MidiRouter router;
    MockMidiOutput mockUsb;
    MockMidiOutput mockDin;
    MockMidiOutput mockInternal;
    router.setOutput(MidiPort::USB_DEVICE, &mockUsb);
    router.setOutput(MidiPort::DIN, &mockDin);
    router.setOutput(MidiPort::INTERNAL, &mockInternal);
    router.setDefaultRoutes();

    ASSERT_EQ(4, router.getRouteCount());

    // Internal NoteOn → USB_DEVICE + DIN
    MidiMessage noteMsg { MidiMessage::NoteOn, 1, 60, 90, MidiPort::INTERNAL };
    router.route(noteMsg);
    ASSERT_EQ(1, mockUsb.noteOnCount());
    ASSERT_EQ(1, mockDin.noteOnCount());
    ASSERT_EQ(0, mockInternal.eventCount());

    mockUsb.reset(); mockDin.reset(); mockInternal.reset();

    // USB_DEVICE NoteOn → INTERNAL
    MidiMessage usbNote { MidiMessage::NoteOn, 2, 64, 80, MidiPort::USB_DEVICE };
    router.route(usbNote);
    ASSERT_EQ(1, mockInternal.noteOnCount());
    ASSERT_EQ(0, mockUsb.eventCount());  // no echo
    ASSERT_EQ(0, mockDin.eventCount());

    mockUsb.reset(); mockDin.reset(); mockInternal.reset();

    // DIN RealTime → INTERNAL
    MidiMessage dinClock { MidiMessage::RealTime, 0, 0xF8, 0, MidiPort::DIN };
    router.route(dinClock);
    ASSERT_EQ(1, mockInternal.eventCount());
}

// ---------------------------------------------------------------------------
// 13. MAX_ROUTES limit
// ---------------------------------------------------------------------------
void testMaxRoutesLimit() {
    MidiRouter router;
    for (uint8_t i = 0; i < MidiRouter::MAX_ROUTES; i++) {
        ASSERT_TRUE(router.addRoute(MidiPort::USB_DEVICE, MidiPort::INTERNAL));
    }
    // Should fail when full
    ASSERT_FALSE(router.addRoute(MidiPort::USB_DEVICE, MidiPort::INTERNAL));
    ASSERT_EQ(MidiRouter::MAX_ROUTES, router.getRouteCount());
}

// ---------------------------------------------------------------------------
// Runner
// ---------------------------------------------------------------------------
void runAll() {
    Serial.println("\n--- Suite: MidiRouter");
    RUN_TEST(testEmptyRouterNoRoutes);
    RUN_TEST(testAddRemoveRoute);
    RUN_TEST(testClearRoutes);
    RUN_TEST(testRouteNoteOnToInternal);
    RUN_TEST(testTypeFilterBlocksCC);
    RUN_TEST(testMultiPortDestination);
    RUN_TEST(testNoEchoToSource);
    RUN_TEST(testChannelRangeFilter);
    RUN_TEST(testDisabledRouteSkipped);
    RUN_TEST(testDestOverrideBypassesTable);
    RUN_TEST(testRealTimeRouting);
    RUN_TEST(testDefaultRoutes);
    RUN_TEST(testMaxRoutesLimit);
    TestFramework::printSummary("MidiRouter");
}

} // namespace TestMidiRouter
