# Multi-Event Steps (Overflow List — D1)

Expressive sub-step recording: flams, rolls, grace notes, strummed chords, melodic runs.

## Architecture

The flat `EventNoteSequence` vector stays as-is (one primary event per step).
A sidecar map holds overflow events for steps that need more than one hit:

```cpp
// In RTPEventNoteSequence (protected)
std::map<uint16_t, vector<RTPEventNotePlus>> _overflowEvents; // key = step position
```

Everything that currently reads the flat vector keeps working unchanged.
Overflow is opt-in — only code that explicitly looks for it sees it.

---

## Implementation phases

### Phase 1 — Data structure + recording
**Goal**: capture multiple events per step during live recording.

1. **Add `_overflowEvents` map** to `RTPEventNoteSequence`
   - Add accessors: `getOverflowEvents(uint16_t step)`, `addOverflowEvent(uint16_t step, RTPEventNotePlus)`, `clearOverflowEvents()`
   - `clearSequence()` must also clear `_overflowEvents`
   - `resizeSequence()` must prune overflow entries beyond the new size

2. **Update `NotesRecorder`** to allow multiple events per step
   - Currently `recordNoteOn` overwrites if two notes land on the same quantized step
   - Change: first event on a step → primary; subsequent events → tagged as overflow
   - Store a flag or separate list in the recorder so the dump knows which are overflow
   - Alternatively: just emit all recorded events and let the dump sort primary vs overflow by earliest timestamp per step

3. **Update `recorderDumpToSequence`** in `LivePlayOrchestrator`
   - For each step: first event → `writeRecordedNote` (primary slot)
   - Additional events on the same step → `addOverflowEvent`
   - Sort events within a step by microOffset so playback order is deterministic

### Phase 2 — Playback
**Goal**: overflow events sound during playback.

4. **Update `playCurrentEventNote`** in each sequence subclass (or add a base-class helper)
   - After playing the primary event, check `_overflowEvents.find(_currentPosition)`
   - For each overflow event whose `microOffset` matches the current pulse counter → queue it
   - Consider a helper `playOverflowEvents()` in the base class to avoid repeating in every subclass

5. **Micro-offset playback window**
   - Overflow events fire on the pulse matching their `microOffset` within the step
   - The existing `isNotePulse()` logic handles the primary event; overflow events need their own per-event pulse check
   - Each `fordwardSequence()` tick (pulse increment) should check overflow events for the current step

### Phase 3 — Persistence
**Goal**: overflow events survive save/load.

6. **JSON serialization** (`dumpSequenceToJson` / load path)
   - Add an `"overflow"` array per step in the JSON, only present when non-empty
   - Each overflow entry: `{ "note", "velocity", "length", "microOffset", "literalPitch" }`
   - On load: populate `_overflowEvents` from the JSON

7. **Binary persistence** (if applicable)
   - Append overflow data after the main sequence block
   - Format: `[step_index: uint16][count: uint8][events...]` repeated for each step with overflow
   - Terminated by sentinel `step_index = 0xFFFF`

### Phase 4 — Editing UI (can be deferred)
**Goal**: visualize and edit overflow events.

8. **Step detail view**
   - When cursor is on a step with overflow, a long-press or dedicated action enters a detail view
   - Shows all events on that step as a mini-list with microOffset, note, velocity
   - 3-axis edits apply to the selected sub-event
   - Delete individual overflow events

9. **Piano roll visualization**
   - Steps with overflow could show a different LED color or brightness to indicate "this step has sub-events"
   - Optional: show overflow count on OLED when hovering a step

### Phase 5 — Musical features (future)
**Goal**: leverage overflow for higher-level musical tools.

10. **Auto-flam**: parameter per step that auto-generates a second hit N pulses before/after with reduced velocity
11. **Roll generator**: fill a step's overflow with evenly spaced retrigs at configurable count and velocity curve
12. **Strum generator**: for poly sequences, stagger chord tones across pulses within a step
13. **Humanize**: slightly randomize microOffset and velocity of overflow events on playback

---

## Key design decisions

| Decision | Choice | Rationale |
|---|---|---|
| Primary vs overflow | First event on step = primary | Backward compat; existing code sees the primary |
| Storage | `std::map<uint16_t, vector<>>` | Only allocates for steps that have overflow; most steps won't |
| Max overflow per step | Soft limit ~8 | Memory safety; 8 sub-events per step is plenty for any musical gesture |
| Playback timing | Each overflow event fires at its own microOffset pulse | True expressive timing, not just even retrigs |
| Editing | Deferred to Phase 4 | Recording and playback deliver value immediately |

## Files touched

| File | Change |
|---|---|
| `include/Sequencer/RTPEventNoteSequence.h` | Add `_overflowEvents`, accessors |
| `src/Sequencer/RTPEventNoteSequence.cpp` | Implement accessors, update clear/resize |
| `src/Helpers/NotesRecorder.cpp` | Allow multiple events per step |
| `src/Helpers/LivePlayOrchestrator.cpp` | Dump overflow events alongside primaries |
| `src/Sequencer/DrumSequence.cpp` | Play overflow in `playCurrentEventNote` |
| `src/Sequencer/MonoSequence.cpp` | Play overflow in `playCurrentEventNote` |
| `src/Sequencer/BassSequence.cpp` | Play overflow in `playCurrentEventNote` |
| `src/Sequencer/PolySequence.cpp` | Play overflow in `playCurrentEventNote` |
| `src/Sequencer/HarmonySequence.cpp` | Play overflow in `playCurrentEventNote` |
| Persistence files | Serialize/deserialize overflow |

## Estimated effort

- **Phase 1 (data + recording)**: 1 session
- **Phase 2 (playback)**: 1 session
- **Phase 3 (persistence)**: 1 session
- **Phase 4 (editing UI)**: 1-2 sessions (deferrable)
- **Phase 5 (musical tools)**: ongoing / as-needed
