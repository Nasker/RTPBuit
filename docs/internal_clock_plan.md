# Internal Clock Generator Implementation Plan

## Overview
Add internal clock generation capability to RTPBuit sequencer, allowing standalone operation without external MIDI sync, while maintaining ability to output MIDI real-time messages to external gear.

## Current Architecture

### External Sync Flow (MIDI)
```
MIDI IN → usbMIDI.read() → linkToSequencerManager() → RTPSequencerManager::handleRealTimeSystem()
    ↓
START/CONTINUE/STOP/CLOCK → _sequencer.playAndMoveSequencer() / stopAndCleanSequencer()
    ↓
UI Callbacks → StateMachine → SceneEditState::presentScene() [display update]
```

### Timing Constants
- `TICKS_PER_BAR = 96` (24 ticks per quarter note × 4 beats)
- `CLOCK_GRID = 6` (steps per 16th note)
- Counter increments on each MIDI clock tick (24 PPQN)

## Proposed Architecture

### New Component: `RTPClockGenerator`
Responsible for generating internal clock pulses and optionally outputting MIDI real-time messages.

```cpp
class RTPClockGenerator {
    enum class ClockMode { External, Internal };
    
    ClockMode _mode = ClockMode::External;
    uint32_t _lastMicros = 0;
    uint32_t _clockIntervalUs = 20833; // 120 BPM default (500000us / 24)
    uint8_t _counter = 0;
    bool _isRunning = false;
    
    // Tap tempo
    uint32_t _tapTimes[4] = {0};
    uint8_t _tapIndex = 0;
    uint32_t _lastTapTime = 0;
    
public:
    void setMode(ClockMode mode);
    void setBPM(float bpm);
    void start();
    void stop();
    void tapTempo();
    void update(); // Call from main loop
    bool checkClockPulse(); // Returns true when clock should fire
    uint8_t getCurrentCounter() const { return _counter; }
    bool isRunning() const { return _isRunning; }
    
    // MIDI output for external sync
    void setSendMidiRealtime(bool enabled);
    
private:
    void sendMidiClock();
    void sendMidiStart();
    void sendMidiStop();
    void calculateTapTempo();
};
```

### Integration Points

1. **RTPMainUnit** - Add `RTPClockGenerator _clockGenerator`
   - Initialize with `SequencerManager` reference
   - Route internal clock pulses to same `handleRealTimeSystem()` path

2. **RTPSequencerManager** - Minimal changes
   - Already handles `handleRealTimeSystem()` - works for both internal/external
   - May need to distinguish source for loopback prevention

3. **TransportState** - UI for transport controls
   - Single click: Play/Stop toggle (when in internal mode)
   - Double click: Scene Edit (existing)
   - Long click: Toggle between Internal/External mode
   - Tap tempo button (via trellis or rotary)

4. **BuitDevicesManager** - Display updates
   - Show clock mode indicator (INT/EXT)
   - Show BPM when in internal mode
   - Transport state indicators (Play ▶ / Stop ■)

### Mode Switching Logic

```cpp
enum class SyncMode {
    External,   // Follow MIDI IN clock
    Internal    // Generate clock internally, output MIDI OUT
};

// In External Mode:
// - Clock from usbMIDI.read() → SequencerManager
// - Ignore internal generator
// - UI shows "EXT" indicator

// In Internal Mode:
// - RTPClockGenerator produces CLOCK pulses
// - Optionally: usbMIDI.sendRealTime(CLOCK) to sync external gear
// - UI shows "INT" + BPM
// - Play/Stop buttons work
```

### Transport State UI (Repurposed)

**NOTE**: TransportState currently handles CC matrix via TOF sensors (`sendBuitCC`). This will move to a new `CCMatrixState` later.

**TransportState becomes the Transport Control Center:**

Entering TransportState shows the transport UI with:

| Trellis Button | Short Press | Long Press |
|----------------|-------------|------------|
| ▶ Play | Start playback (internal mode) | - |
| ■ Stop | Stop playback | - |
| ● Rec | Toggle recording | Punch-in mode |
| ⌛ Tap | Tap tempo | Clear tempo |
| 🔄 Mode | Toggle INT/EXT sync | - |

**Display Layout (Transport State):**
```
┌─────────────────────────┐
│ Transport    INT/EXT   │  <- Mode indicator
│ ▶/■  120 BPM           │  <- Play state + BPM
│ Scene 2  [Rec:Waiting] │  <- Current scene + rec status
│                         │
└─────────────────────────┘
```

**State Transition:**
- Double-click from Transport → Scene Edit (existing)
- From Scene Edit → single click → Transport (new shortcut)

### Display Layout (Scene Edit)

```
┌─────────────────────────┐
│ Scene  ▶/■    INT/EXT   │  <- Play state + Clock mode
│ Scene 2      120 BPM    │  <- Scene name + BPM (if internal)
│                         │
│                         │
└─────────────────────────┘
```

## Implementation Phases

### Phase 1: Core Clock Generator
1. Create `RTPClockGenerator.hpp/cpp`
2. Implement microsecond-based timing
3. Add BPM calculation
4. Test standalone timing accuracy

### Phase 2: Mode Switching
1. Add `SyncMode` enum to constants
2. Integrate clock generator into `RTPMainUnit`
3. Add mode toggle logic
4. Ensure clean handoff between modes (reset counter on switch)

### Phase 3: MIDI Output (Internal Master)
1. Add `setSendMidiRealtime(bool)` to clock generator
2. Send START/STOP/CLOCK to `usbMIDI` when internal mode active
3. Prevent MIDI feedback (don't read our own output)

### Phase 4: Transport UI
1. Extend `TransportState` with button handlers
2. Add tap tempo logic to `RTPClockGenerator`
3. Update `BuitDevicesManager` display methods
4. Test button interactions

### Phase 5: Recording Integration
1. Ensure recording works with internal clock
2. Verify timing accuracy of recorded notes
3. Test sync with external gear (RTPBuit as master)

## Key Design Decisions

### 1. Shared Interface
Both internal and external clocks use the same `handleRealTimeSystem()` method, ensuring identical behavior.

### 2. Clock Source Priority
- External MIDI always takes precedence when in External mode
- Switching to Internal mode stops external sync immediately
- Switching to External mode resets and waits for next START

### 3. Tap Tempo Algorithm
- Store last 4 tap intervals
- Average valid taps (300ms - 2000ms range)
- Reject outlier taps (>2x or <0.5x average)
- Reset after 2 seconds of no taps

### 4. BPM Range
- Minimum: 40 BPM (104167 µs per clock)
- Maximum: 240 BPM (17361 µs per clock)
- Default: 120 BPM

## SOLID Principles Application

- **Single Responsibility**: Clock generator only creates timing pulses
- **Open/Closed**: New clock sources can be added (e.g., CV clock)
- **Liskov**: Internal/External clocks interchangeable via same interface
- **Interface Segregation**: Clock generator minimal API
- **Dependency Inversion**: Sequencer depends on abstract "clock source" concept

## Testing Checklist

- [ ] Internal clock accuracy (±1% at 120 BPM)
- [ ] Clean mode switching without glitches
- [ ] Tap tempo responsiveness
- [ ] MIDI output timing (if enabled)
- [ ] External sync still works when switched back
- [ ] Display updates correctly in all modes
- [ ] Recording timing accurate in internal mode
- [ ] Transport buttons work in all states
