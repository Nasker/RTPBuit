# RTPBuit Architecture Documentation

## Overview

RTPBuit is a sophisticated MIDI sequencer and performance instrument built for Teensy 4.1, featuring multi-axis sensor control, chord-based music theory, and a hierarchical state machine architecture. The system processes real-time sensor input, manages multiple sequencer tracks, and outputs MIDI across USB, USB Host, and hardware serial interfaces.

## Core Architecture

### Main Application Flow

The application follows a clear hierarchical structure centered around `RTPMainUnit` as the central orchestrator:

```
main.cpp
├── RTPMainUnit (Central Coordinator)
│   ├── Hardware Layer (Devices)
│   ├── Sequencer Layer (Music Generation)
│   ├── State Machine Layer (UI/Control)
│   └── Helper Layer (Utilities)
```

**Entry Points:**
- `setup()`: Initializes all subsystems and MIDI handlers
- `loop()`: Processes MIDI input, updates main unit, handles periodic sensor reads
- `actOnPeriodicUpdate()`: Called every 10ms for three-axis sensor processing

### Hardware Abstraction Layer

#### Adapter Pattern for Legacy Hardware

The system uses the **Adapter Pattern** to bridge legacy hardware classes to modern interfaces:

```
Legacy Hardware          Adapter                 Interface
───────────────         ────────                ─────────
RTPOled          →  RTPOledAdapter       →  IDisplay
RTPNeoTrellis    →  RTPNeoTrellisAdapter →  IButtonMatrix
RTPRotaryClickDev →  RTPRotaryAdapter     →  IRotaryEncoder
RTPThreeAxisVL   →  RTPThreeAxisAdapter  →  IThreeAxisSensor
```

**Production wiring (RTPMainUnit):**
- One `RTPOled` instance and one `RTPNeoTrellis` instance exist; both are shared by:
  - `DeviceManager` → `DisplayManager` / `InputManager` (via `shared_ptr` to the adapters)
  - `BuitDevicesManager` (state-machine facade) — depends **only** on `IDisplay&` / `IButtonMatrix&`, constructor-injected with the same adapters
- OLED is initialized once via `DeviceManager::initialize()` → `DisplayManager` → `RTPOledAdapter::initialize()`
- Hardware **polling** stays in the legacy path (`rtpTrellis.read()`, `rtpRotary.read()`, `vlSensor`); `DeviceManager::update()` is intentionally not called to avoid double-polling shared hardware

**Benefits:**
- Single driver instance per physical device (previously two `U8G2` drivers drove one display)
- State machine code depends on interfaces, enabling mock-based tests
- Decomposed managers (DisplayManager, InputManager, TransportManager) operate on the same live hardware without duplication
- Facilitates testing with mock implementations

**Interface surface:** `IDisplay` includes `printFourLinesWithState(...)` for the state-aware pages (play/stop/rec indicators). `IButtonMatrix` includes the domain page writers (`writeSequenceStates`, `writeSceneStates`, `writeBuitCCStates`, `writeSequenceSettingsPage`, `moveCursor`, `introAnimation`) and `get*` color helpers used by the UI states.

**Location:** `/include/Hardware/Adapters/`

#### Device Drivers (`src/Devices/`)

**RTPThreeAxisVL** - Three-Axis Distance Sensing
- Uses VL53L0X time-of-flight sensors for hand position tracking
- Provides cleaned readings with configurable min/max limits
- Emits presence-lost events when hand leaves sensor range
- Critical for live performance control and arpeggiation

**RTPNeoTrellis** - 4x4 LED Button Grid
- Provides visual feedback and user input
- Supports individual button color control and animations
- Handles press/release events with callback system
- Used for pattern selection, note triggering, and UI navigation

**RTPOled** - OLED Display (128x64)
- U8G2-based display driver for SH1106 OLED
- Shows multi-line text with recording indicators
- Provides context-aware display updates for different states

**RTPRotaryClickChordion** - Rotary Encoder with Click
- Handles rotary rotation and button press events
- Used for parameter adjustment and navigation
- Supports single, double, triple, and long click detection

### Sequencer Architecture

#### Hierarchical Organization

```
RTPSequencer implements ISequencer (Top Level)
├── RTPScene[] (dynamic, min 1)
│   └── RTPEventNoteSequence[] (16 sequences per scene)
│       ├── DrumSequence     → IMidiOutput (sendNoteOn/Off)
│       ├── BassSequence     → IMidiOutput (latch/legato/roll)
│       ├── MonoSequence     → IMidiOutput (latch/legato/roll)
│       ├── PolySequence     → IMidiOutput (chord voicing)
│       ├── ControlSequence
│       └── HarmonySequence  → IMidiOutput (sendControlChange)
│
IClockGenerator ← RTPClockGenerator (Internal / External sync)
```

#### Sequence Types and Behaviors

**DrumSequence**: Simple trigger-based drum sequencer
- Note on/off events with velocity via `IMidiOutput`
- No chord processing or arpeggiation
- Live play routed through interface (testable with MockMidiOutput)

**BassSequence**: Monophonic bass with advanced features
- **Latch Mechanism**: Root press snapshots chord and holds it
- **Legato Transitions**: Overlapping note-on/off for smooth glides via `IMidiOutput`
- **Presence Gating**: Only sounds when left/center axis present
- **Arpeggiation**: Unfolds chords across 3 octaves (BASS_OCTAVES)
- **Hysteresis**: Prevents flutter near slot boundaries
- **Roll Mode**: 32nd-note retrigger on half-ticks via `IMidiOutput`

**MonoSequence**: Monophonic lead synthesizer
- Similar to BassSequence but with 5-octave arpeggiation (SYNTH_OCTAVES)
- Same latch, legato, presence gating, and roll features via `IMidiOutput`

**PolySequence**: Polyphonic chord synthesizer
- Momentary chord playback (no latch)
- Full chord voicing via `IMidiOutput`
- Defensive note-off queue clears stuck notes before new chords

#### Event Flow and Timing

**Clock Generation and Synchronization**
```
RTPClockGenerator
├── Internal Mode: Generates MIDI clock, outputs to USB/Serial
└── External Mode: Follows incoming MIDI clock
```

**Timing Resolution**
- Standard 24 PPQN (pulses per quarter note)
- Fine 32nd-note resolution for live rolls (GRID_FINE_TICK)
- Swing and quantization support

**Event Processing Pipeline**
```
MIDI Clock → RTPSequencerManager (IClockGenerator) → ISequencer::play()
    → Scene → Sequence → NotesPlayer / IMidiOutput → TeensyMidiOutput (usbMIDI + Serial1)
```

### State Machine Architecture

#### State Hierarchy

The system uses a hierarchical state machine pattern with `BuitStateMachine` managing multiple `BuitState` implementations:

**Core States:**
- `InitState`: System initialization and splash screens
- `TransportState`: Playback control, tempo, swing, quantization
- `SceneEditState`: Scene management and selection
- `SequenceEditState`: Note editing and pattern creation
- `SequencePianoRollState`: Live performance and recording
- `SequenceSelectState`: Track selection and configuration
- `GlobalSettingsState`: System-wide configuration

**State Transition Pattern:**
```
BuitStateMachine
├── Handles all input events
├── Delegates to current state
└── Manages state transitions
```

#### Event Handling

All states implement the `BuitState` interface with standardized event handlers:
- `singleClick()`, `doubleClick()`, `tripleClick()`, `longClick()`
- `rotaryTurned()`, `threeAxisChanged()`
- `trellisPressed()`, `trellisReleased()`
- `sequencerCallback()`, `midiNote()`, `midiCC()`

### Music Theory and Chord System

#### ChordionKeys System

**Chord Management:**
- Supports up to 16 simultaneous active chords
- 4 modifier keys (pads 12-15) for chord type selection
- Two modes: SWEEP (autoharp-style) and POLYPHONIC

**Chord Action API:**
```cpp
ChordAction beginChord(rootNote, padIndex);  // Start chord
ChordAction endChord(rootNote, padIndex);    // End chord
```

**Live Play Behavior:**
- **Poly**: Momentary full chords, release on pad release
- **Bass/Mono**: Latched single chord, persists after release

#### MusicManager Integration

**Scale and Harmony System:**
- MIDI CC mapping for root note and scale selection
- Real-time harmony updates via `setCurrentHarmony()`
- Smart range mapping for different sequence types

**Range Mapping:**
- Bass: 3 octaves, 7 steps per octave
- Mono: 5 octaves, 7 steps per octave  
- Poly: 4 octaves, 7 steps per octave

### MIDI and Audio Output Paths

#### Multi-Interface MIDI Support

**Input Sources:**
- USB MIDI (device mode)
- USB Host MIDI (host mode for external controllers)
- Hardware Serial MIDI (5-pin DIN on Serial1)

**Output Destinations:**
- USB MIDI (device mode)
- Hardware Serial MIDI (Serial1 at 31250 baud)
- USB Host MIDI (when in internal clock mode)

#### Note Output Pipeline

```
Sequence Event → NotesPlayer Queue / IMidiOutput → TeensyMidiOutput → USB MIDI + Serial1
```

**IMidiOutput Interface:**
- Single abstraction point for all MIDI output (NoteOn/Off, CC, PC, PitchBend, RealTime, Raw)
- `TeensyMidiOutput` is the sole concrete implementation (handles usbMIDI + Serial1)
- `MockMidiOutput` enables unit testing without hardware
- Injected via `RTPMainUnit` → `RTPSequencer::setMidiOutput()` → `RTPScene` → each `RTPEventNoteSequence`

**NotesPlayer Features:**
- Per-channel note tracking to prevent duplicate notes
- Time-to-live management for note duration
- All-notes-off emergency stop capability

**RTPEventNotePlus:**
- Encapsulates note, velocity, channel, duration
- Supports literal pitch mode for direct MIDI output
- Automatic note-off after time-to-live expires

### Control Command System

#### Unified Event Architecture

All user input and system events flow through the `ControlCommand` structure:

```cpp
struct ControlCommand {
    int controlType;  // Source: TRELLIS, ROTARY, THREE_AXIS, MIDI, etc.
    int commandType;  // Action: PRESSED, ROTATING_LEFT, CHANGE_X, etc.
    int value;        // Parameter: button index, sensor reading, etc.
};
```

#### Event Routing

```
Hardware Input → ControlCommand → StateMachineManager → Current State → Action
```

### Advanced Performance Features

#### Three-Axis Control Integration

**Live Performance Mapping:**
- Left/Center sensors: Note selection and arpeggiation
- Right sensor: Parameter control (modulation, filter, etc.)
- Presence detection for expressive control

**Sensor Processing:**
- Continuous reading at 100Hz (10ms intervals)
- Noise filtering and range normalization
- Presence-lost event generation for gating

#### Recording and Quantization

**NotesRecorder Integration:**
- Real-time note capture during live performance
- Quantization strength adjustment (0-100%)
- Pattern bank storage and retrieval

**Recording States:**
- Waiting: Armed for recording at position 0
- Recording: Active note capture
- Playing: Normal playback mode

### Persistence and Configuration

#### Storage Management

**BuitPersistenceManager:**
- JSON-based pattern storage on SD card
- Scene and sequence serialization
- Configuration backup and restore

**Pattern Bank System:**
- 16 pages × 16 slots = 256 pattern locations
- Visual feedback with color-coded existence indicators
- Quick load/save operations

### Clock and Synchronization

#### RTPClockGenerator Features

**Dual Mode Operation:**
- Internal: Tap tempo, BPM adjustment, MIDI clock output
- External: Slave to incoming MIDI clock

**Tap Tempo System:**
- 4-tap averaging with timeout handling
- 40-240 BPM range with 1 BPM resolution
- Visual and haptic feedback

### Memory Management and Performance

#### Resource Allocation

**Dynamic Scene Management:**
- Runtime scene addition/removal
- Memory cleanup on scene deletion
- Minimum 1 scene requirement enforcement

**Efficient Data Structures:**
- Vector-based scene storage with unique_ptr management
- Ring buffer patterns for note tracking
- Queue-based event processing

## Key Design Patterns

### Hierarchical Composition
- Clear separation between hardware, sequencer, and UI layers
- Dependency injection through constructor parameters
- Centralized coordination through RTPMainUnit

### State Machine Pattern
- Polymorphic state handling with uniform interface
- Context preservation during state transitions
- Event-driven architecture with command pattern

### Observer Pattern
- Callback-based event propagation
- Loose coupling between components
- Real-time responsive system design

### Strategy Pattern
- Pluggable sequence types with specialized behaviors
- Configurable clock generation modes
- Flexible chord generation algorithms

## Performance Considerations

### Real-Time Constraints
- 10ms sensor update cycle
- Sub-millisecond MIDI timing accuracy
- Immediate visual feedback on all interactions

### Memory Efficiency
- Smart pointer usage for automatic cleanup
- Minimal dynamic allocation during performance
- Efficient event queue management

### Scalability
- Configurable scene and sequence counts
- Extensible sequence type system
- Modular hardware abstraction

This architecture enables RTPBuit to function as both a sophisticated studio sequencer and an expressive live performance instrument, with the three-axis sensor system providing unique musical control possibilities not found in traditional hardware.
