# MIDI Router – Implementation Plan

## Overview

Turn the BUIT into a full MIDI router. The Teensy 4.1 already has three physical MIDI transports available:

| Port ID | Transport | Direction | Teensy API |
|---------|-----------|-----------|------------|
| `USB_DEVICE` | USB MIDI (device mode – cable to DAW/host) | IN + OUT | `usbMIDI` |
| `USB_HOST` | USB Host port (external synths/controllers plug in) | IN + OUT | `USBHost_t36` → `MIDIDevice` |
| `DIN` | 5-pin DIN via Serial1 (31 250 baud) | IN + OUT | `Serial1` |
| `INTERNAL` | Sequencer, CC automation, clock generator, etc. | IN + OUT | Software (existing managers) |

The goal is a **routing matrix**: any MIDI message from any port can be forwarded to any combination of other ports, filtered by message type and channel. The sequencer's per-track "MIDI Channel" parameter gets extended to also specify the **output port**, so a sequence can target, for example, channel 3 on the USB Host synth while another targets channel 10 on DIN.

---

## Current Pain Points

1. **Hard-coded outputs** – `RTPEventNotePlus::playNoteOn/Off()` directly calls `usbMIDI.sendNoteOn()` and `Serial1.write()`. There is no way to choose *which* port a note goes to.
2. **No input demux** – `main.cpp` routes all three inputs (USB device, USB Host, DIN) through the same `routeNoteOn/Off` and `routeControlChange` callbacks with no port-of-origin tag.
3. **Clock forwarding is fixed** – `RTPSequencerManager::dispatchRealTime()` always sends to both `usbMIDI` and `Serial1`; no option to send clock only to DIN or only to USB Host.
4. **USB Host is read-only** – `midi1` (`MIDIDevice`) is read via handlers but never used as an output destination.
5. **No device enumeration** – connected USB Host devices aren't listed or identified.

---

## Architecture

### 1. `MidiPort` – Enum & Bitmask

```cpp
// include/Midi/MidiPort.hpp
#pragma once
#include <cstdint>

enum class MidiPort : uint8_t {
    USB_DEVICE = 0x01,
    USB_HOST   = 0x02,
    DIN        = 0x04,
    INTERNAL   = 0x08,
    ALL        = 0x0F
};

inline MidiPort operator|(MidiPort a, MidiPort b) { return static_cast<MidiPort>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b)); }
inline bool operator&(MidiPort a, MidiPort b) { return static_cast<uint8_t>(a) & static_cast<uint8_t>(b); }
```

Using a bitmask lets a single route entry target multiple outputs cheaply.

### 2. `MidiMessage` – Canonical message struct

```cpp
// include/Midi/MidiMessage.hpp
struct MidiMessage {
    enum Type : uint8_t { NoteOn, NoteOff, CC, ProgramChange, PitchBend, RealTime, SysEx, Raw };
    Type type;
    uint8_t channel;   // 1-16, 0 = N/A (real-time)
    uint8_t data1;     // note / CC# / program / RT byte
    uint8_t data2;     // velocity / value / 0
    MidiPort source;   // where the message came from
};
```

### 3. `MidiRouter` – Central routing engine

```
include/Midi/MidiRouter.hpp
src/Midi/MidiRouter.cpp
```

Responsibilities:
- Owns a **routing table** (`std::array` of `RouteEntry` structs, one per logical route).
- Exposes `void route(const MidiMessage& msg)` – the single entry point every input calls.
- For each route entry whose filter matches (source port, message type, channel range), forwards the message to the designated output port(s) by calling the appropriate `IMidiOutput` implementation.
- Provides helpers: `addRoute()`, `removeRoute()`, `clearRoutes()`, `setDefaultRoutes()`.

```cpp
struct RouteEntry {
    MidiPort   sourceMask;     // which input port(s) this rule applies to
    MidiPort   destMask;       // where to send
    uint8_t    msgTypeMask;    // bitmask of MidiMessage::Type (0xFF = all)
    uint8_t    channelMin;     // 1-16, 0 = any
    uint8_t    channelMax;     // 1-16, 0 = any
    bool       enabled;
};
```

Default routes (backward-compatible):
| Source | Dest | Filter |
|--------|------|--------|
| `INTERNAL` | `USB_DEVICE \| DIN` | All messages |
| `USB_DEVICE` | `INTERNAL` | Notes, CC, Real-Time |
| `USB_HOST` | `INTERNAL` | Notes, CC |
| `DIN` | `INTERNAL` | Real-Time |

### 4. `IMidiOutput` per port

The existing `TeensyMidiOutput` currently wraps both USB device and Serial1. Refactor into:

| Class | Port | Notes |
|-------|------|-------|
| `UsbDeviceMidiOutput` | `USB_DEVICE` | Wraps `usbMIDI.*` |
| `UsbHostMidiOutput` | `USB_HOST` | Wraps `MIDIDevice` (midi1) – needs pointer to `MIDIDevice` instance from main |
| `DinMidiOutput` | `DIN` | Wraps `Serial1` |
| `InternalMidiSink` | `INTERNAL` | Calls back into `RTPMainUnit::routeControlChange` / `routeNoteOnOff` / `linkToSequencerManager` |

All implement `IMidiOutput`. `MidiRouter` holds a `std::array<IMidiOutput*, 4>` indexed by port enum.

### 5. USB Host Device Enumeration

Teensy's `USBHost_t36` library supports hot-plug callbacks. Add:

```cpp
// include/Midi/UsbHostManager.hpp
class UsbHostManager {
    USBHost& _usb;
    MIDIDevice* _devices[MAX_USB_HOST_DEVICES]; // up to 4 on Teensy 4.1 hub
    uint8_t _deviceCount;
public:
    void begin(USBHost& usb);
    void update();                     // call in loop()
    uint8_t getDeviceCount() const;
    String getDeviceName(uint8_t idx); // product string if available
    MIDIDevice* getDevice(uint8_t idx);
};
```

This list can be exposed to the UI so the user sees "Synth X connected on USB Host port 1".

### 6. Extend Sequence Track Routing

Currently each `RTPEventNoteSequence` has a `parameterMidiChannel` (1-16). Extend with a **port destination** parameter:

```
sequenceParameters.push_back(RTPParameter(0, 3, 0, "Port"));  // 0=USB_DEV, 1=USB_HOST, 2=DIN, 3=ALL
```

When the sequencer plays a note, instead of calling `usbMIDI.sendNoteOn()` directly, it calls:

```cpp
MidiMessage msg { MidiMessage::NoteOn, channel, note, velocity, MidiPort::INTERNAL };
msg.destOverride = getPortFromParameter();   // sequence-level routing
midiRouter.route(msg);
```

This means the user can set Sequence 3 → Channel 5 → USB Host (their external synth), while Sequence 1 → Channel 10 → DIN (drum machine), all from the existing per-sequence parameter UI.

### 7. Decouple `RTPEventNotePlus` from hardware

**This is the most important refactor.** `playNoteOn()` / `playNoteOff()` currently call `usbMIDI` and `Serial1` directly. Replace with an injected callback or pointer:

```cpp
// Option A: static function pointer (low overhead, Teensy-friendly)
class RTPEventNotePlus {
    static void (*sendNoteOnFn)(uint8_t note, uint8_t velocity, uint8_t channel, MidiPort dest);
    static void (*sendNoteOffFn)(uint8_t note, uint8_t channel, MidiPort dest);
    MidiPort _destPort = MidiPort::ALL;
    ...
};
```

Set once at startup: `RTPEventNotePlus::sendNoteOnFn = [](auto n, auto v, auto ch, auto p){ midiRouter.route({NoteOn, ch, n, v, INTERNAL}); };`

This eliminates the direct hardware dependency from the note/sequencer layer entirely.

### 8. Real-Time Clock Routing

`RTPSequencerManager::dispatchRealTime()` currently hardcodes `usbMIDI.sendRealTime()` + `Serial1.write()`. Change to:

```cpp
void RTPSequencerManager::dispatchRealTime(uint8_t realtimebyte) {
    handleRealTimeSystem(realtimebyte);
    if (_clockGenerator && _clockGenerator->isSendingMidiRealtime()) {
        MidiMessage msg { MidiMessage::RealTime, 0, realtimebyte, 0, MidiPort::INTERNAL };
        _midiRouter->route(msg);  // router decides where clock goes
    }
}
```

The router table controls whether clock goes to DIN only, USB Host only, all ports, etc.

### 9. Input Parsing Improvements

The current DIN input parsing in `main.cpp` only catches real-time bytes (`>= 0xF8`). Implement a proper MIDI byte parser (state machine) for Serial1 so that Note On/Off, CC, etc. from DIN are fully parsed and routed:

```cpp
// include/Midi/MidiParser.hpp
class MidiParser {
    uint8_t _runningStatus = 0;
    uint8_t _buffer[3];
    uint8_t _expected = 0;
    uint8_t _count = 0;
public:
    // Returns true when a complete message is available
    bool feed(uint8_t byte, MidiMessage& out);
};
```

Used in `loop()`:

```cpp
while (Serial1.available()) {
    MidiMessage msg;
    if (dinParser.feed(Serial1.read(), msg)) {
        msg.source = MidiPort::DIN;
        midiRouter.route(msg);
    }
}
```

---

## Implementation Phases

### Phase 1 – Foundation (non-breaking) ✅
1. ✅ Create `MidiPort` enum + bitmask operators.
2. ✅ Create `MidiMessage` struct.
3. ✅ Create `MidiRouter` class with routing table and `route()` method.
4. ✅ Create per-port `IMidiOutput` implementations (`UsbDeviceMidiOutput`, `DinMidiOutput`).
5. ✅ Wire `MidiRouter` into `RTPMainUnit`; keep existing direct calls as fallback.
6. ✅ Unit tests for `MidiRouter` matching logic.

### Phase 2 – Decouple note output ✅
7. ✅ Remove direct `usbMIDI` / `Serial1` calls from `RTPEventNotePlus::playNoteOn/Off()`.
8. ✅ Inject routing function pointer (or `MidiRouter*` reference) into `RTPEventNotePlus`.
9. ✅ Add `_destPort` member to `RTPEventNotePlus` (set from sequence parameters).
10. ✅ Update `NotesPlayer` to pass port destination through.
11. ✅ Verify all existing sequences still play correctly on USB Device + DIN (default route).

### Phase 3 – USB Host output + enumeration ✅
12. ✅ Create `UsbHostMidiOutput` wrapping `MIDIDevice::send*()`.
13. ✅ Create `UsbHostManager` for device enumeration / hot-plug.
14. ✅ Register USB Host as output port in `MidiRouter`.
15. ⏳ Display connected USB Host device name on OLED (deferred to UI polish).

### Phase 4 – Per-sequence port assignment ✅
16. ✅ Add "Port" parameter to `RTPEventNoteSequence` parameters list.
17. ✅ Propagate port selection into `RTPEventNotePlus::_destPort` when playing.
18. ✅ UI: expose port parameter alongside MIDI channel in the sequence parameter editor.
19. ⏳ If USB Host device is connected, show its name as the port label (deferred to UI polish).

### Phase 5 – Clock & real-time routing ✅
20. ✅ Refactor `RTPSequencerManager::dispatchRealTime()` to go through `MidiRouter` (done in Phase 2).
21. ✅ Add user-configurable "Clock Output" route (which ports receive clock).
22. ✅ Add user-configurable "Clock Input" source selection (USB Device, DIN, USB Host).
23. ✅ External sync via USB Host (if a master clock comes from a USB device).

### Phase 6 – Full DIN input parsing ✅
24. ✅ Implement `MidiParser` state machine for Serial1 bytes.
25. ✅ Replace the raw `while (Serial1.available())` block in `main.cpp` with `MidiParser`.
26. ✅ Route parsed DIN messages through `MidiRouter`.

### Phase 7 – CC / Automation routing ✅
27. ✅ Route all live play (NoteOn/Off, CC) through `MidiRouter` via `routeLiveNoteOn/Off/CC`.
28. ✅ All sequence types use per-sequence port assignment for live play.
29. ✅ Incoming external CCs routed to internal parameters via `InternalMidiSink`.

### Phase 8 – Persistence & UI polish ✅
30. ✅ Save/load routing config (clock ports) to SD card via `BuitPersistenceManager`.
30b. ✅ Per-sequence Port parameter persisted in sequence JSON (`"p"` key).
31. ⏳ Add a "MIDI Routing" state/page in the `StateMachineManager` for visual matrix editing (future).
32. ⏳ Show active routes on the NeoTrellis grid (source rows × dest columns) (future).

---

## File Structure (new files)

```
include/Midi/
    MidiPort.hpp          – Port enum + bitmask
    MidiMessage.hpp       – Canonical message struct
    MidiRouter.hpp        – Routing engine
    MidiParser.hpp        – DIN byte parser
    UsbHostManager.hpp    – USB Host enumeration
    UsbDeviceMidiOutput.hpp
    UsbHostMidiOutput.hpp
    DinMidiOutput.hpp
    InternalMidiSink.hpp

src/Midi/
    MidiRouter.cpp
    MidiParser.cpp
    UsbHostManager.cpp
    UsbDeviceMidiOutput.cpp
    UsbHostMidiOutput.cpp
    DinMidiOutput.cpp
    InternalMidiSink.cpp

test/Tests/
    MidiRouterTest.cpp
    MidiParserTest.cpp
```

---

## Key Design Decisions

- **Bitmask routing** – Avoids dynamic allocation; fits in a fixed-size array on Teensy.
- **Static function pointers** in `RTPEventNotePlus` – Minimal overhead, no virtual dispatch per note, Teensy-friendly.
- **Backward compatibility** – Default routes replicate current behavior. Nothing breaks until Phase 2.
- **MidiRouter is not a Manager** – It sits below the manager layer. Managers configure routes; `MidiRouter` just forwards. This keeps it testable and decoupled.
- **One `MidiRouter` instance** – Owned by `RTPMainUnit`, passed by pointer/reference to whatever needs it.

---

## Constraints & Considerations

- **Teensy 4.1 USB Host** supports up to 4 USB devices via a hub (`USBHost_t36` library). `MIDIDevice` instances must be declared statically at compile time. Start with 1 (`midi1`), expand to 2-4 if needed.
- **Serial1 full-duplex** – DIN MIDI is already wired on pins 0 (RX) and 1 (TX). No additional hardware needed.
- **Latency** – The router is a thin dispatch layer; no buffering or queuing. Messages are forwarded in the same `loop()` iteration they arrive.
- **Memory** – Routing table of 16 entries × 6 bytes = 96 bytes. Negligible on Teensy 4.1's 1 MB RAM.
- **`USB_MIDI_SERIAL` build flag** is already set in `platformio.ini`, enabling both USB MIDI and Serial simultaneously.
