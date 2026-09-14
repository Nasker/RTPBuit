# Per-Lane Clock Divider (Polymeter/Polyrhythm) – Implementation Plan

## Overview

Today every `RTPEventNoteSequence` in every scene steps at the same fixed
rate: one position per master tick, where a master tick is gated to 16th-note
resolution (`RTPSequencerManager::gridClockUp`, `CLOCK_GRID = 6` of the 24
PPQN raw clock — see `include/Config/MusicConfig.hpp`). The one exception is
`HarmonySequence`, which overrides `fordwardSequence()` with a private
`_countTracker`/`N_COUNTS = 4` divider to advance once per **quarter note**
instead of once per 16th note (`src/Sequencer/HarmonySequence.cpp:8-16`).

Goal: generalize that divider into a per-sequence, user-selectable parameter
(default = current 16th-note behavior, unchanged), and extend the master
clock dispatch so lanes can also run **faster** than 16th notes (e.g.
triplet-16th hi-hat rolls), not just slower. This turns every lane's timing
into an independent, composable clock — real polymeter/polyrhythm per track,
not just the differing-loop-length polymeter already used in
`midi_to_pattern/gen_pattern_dark.py` (which staggers loop *length*, not
step *speed*).

---

## Current Limitations

1. **Divider logic is hardcoded to one subclass.** `HarmonySequence` is the
   only sequence type with non-1:1 tick-to-step behavior, and it's baked
   into a subclass override rather than being a configurable field.
2. **Nothing steps faster than 16th notes.** `RTPSequencer::playAndMoveSequencer()`
   is only invoked when `counter % CLOCK_GRID == 0` in `gridClockUp`
   (`src/Sequencer/RTPSequencerManager.cpp:75-95`). The existing finer grid
   (`FINE_GRID = 3`, 32nd-note resolution, `GRID_FINE_TICK`) is wired only to
   the live drum-roll feature (`LivePlayOrchestrator::handleLiveFineTick`)
   and never advances a sequence's `_currentPosition`.
3. **Note length/TTL is implicitly counted in master-tick units.**
   `NotesPlayer::decreaseTimeToLive()` is called once per scene, per master
   tick, from inside `playAndMoveSequencer()`'s scene loop
   (`src/Sequencer/RTPSequencer.cpp:21-29`). A packed note's `length`/`ttl`
   (4-bit field, max 15) means "N master ticks," which today is a single,
   shared unit across every lane. Per-lane clock rates break that
   assumption unless handled deliberately (see Design Concern #1 below).
4. **Binary format has no room for a per-sequence divider field.** The
   8-byte sequence header (`type, midiChannel, color, lengthPages,
   inputPort, port, nNotes`) has no spare field.

---

## Design

### 1. `_clockDivider` on the base `RTPEventNoteSequence` — fixed enum, not a raw integer

**Resolved design decision:** the parameter is **not** an arbitrary integer
the user can dial in. It's an index into a fixed, named table of musically
meaningful grids (unit: raw 24-PPQN clock pulses internally, so both
slower- and faster-than-16th grids reuse the same underlying mechanism):

| Index | Pulses | Musical grid | Divides `TICKS_PER_BAR`(96) evenly? |
|-------|--------|--------------|:---:|
| 0 | 96 | Whole note | yes (1/bar) |
| 1 | 48 | Half note | yes |
| 2 | 24 | Quarter note (replaces `HarmonySequence::N_COUNTS`) | yes |
| 3 | 16 | Quarter-note triplet | yes |
| 4 | 12 | Eighth note | yes |
| 5 | 8  | Eighth-note triplet | yes |
| 6 | 6  | **Sixteenth note — current default for all non-harmony lanes** | yes |
| 7 | 4  | Sixteenth-note triplet (the "1/24" feel referenced in discussion) | yes |
| 8 | 3  | Thirty-second note | yes |
| 9 | 2  | Thirty-second-note triplet | yes |
| 10 | 1 | Sixty-fourth note (fastest possible — one raw pulse per step) | yes |

Every entry divides `TICKS_PER_BAR = 96` with no remainder, so every grid
realigns to the top of the bar — no drift, regardless of how long the
pattern runs. Straight/triplet pairs at each level cover the practically
useful range (hi-hat rolls, triplet basslines, etc.) without opening the
door to an arbitrary divider that means nothing musically. Dotted-note
values (36, 18 pulses) were deliberately **excluded** from v1 — they don't
divide 96 evenly, so a dotted-grid lane would slowly rotate relative to the
bar (real hemiola behavior, not a bug) — worth revisiting as an explicit
"advanced/experimental" option later, but out of scope for the initial,
safe-by-construction enum.

Expose it as a normal `RTPParameter` (range `0..10`, same pattern as the
existing `Type`, `Midi CH`, `Color`, `Length`, `Input`, `Output` parameters
in `src/Sequencer/RTPEventNoteSequence.cpp:18-29`) storing the **enum
index**, not the raw pulse count — the firmware looks up the pulse count
from a small `static const uint8_t CLOCK_DIVIDER_PULSES[11]` table. Default
index `6` (sixteenth note), so every existing pattern (including files
already on disk) behaves identically unless explicitly changed. Loading a
persisted file also **clamps/validates** the stored index against the table
bounds, so a corrupted or hand-crafted binary can't set `_clockDivider` to a
nonsense value at runtime.

### 2. Generalize `fordwardSequence()`, delete `HarmonySequence`'s override

```cpp
// RTPEventNoteSequence.cpp
void RTPEventNoteSequence::fordwardSequence(){
  _pulseCounter++;
  if (_pulseCounter < _clockDivider) return;   // cheap, first-thing check
  _pulseCounter = 0;
  _currentPosition++;
  if (_currentPosition >= getSequenceSize())
    _currentPosition = 0;
}
```

`HarmonySequence` becomes just `RTPEventNoteSequence` constructed with
`_clockDivider = 24` — its dedicated override and `_countTracker`/`N_COUNTS`
can be deleted entirely. Net simplification, not just a new feature.

### 3. Drive `fordwardScene()`/`playScene()` from the raw pulse, not the 16th-note grid

`RTPSequencerManager::gridClockUp` currently gates the *entire* sequencer
behind `CLOCK_GRID`. Change it to call the scene advance on **every raw
pulse**, and let each sequence's own `_clockDivider` check (above) decide
whether this pulse is actually its step:

```cpp
void RTPSequencerManager::gridClockUp(uint8_t realtimebyte){
    _sequencer.play();   // every raw pulse now; per-lane divider gates the real work

    // Existing 16th-note GRID_TICK callback stays as-is, still gated at
    // CLOCK_GRID, for UI/trellis sync (see Performance Considerations).
    if (counter % MusicConfig::Timing::CLOCK_GRID == 0){
        ControlCommand callbackCommand;
        ...
    }
    increaseCounter();
}
```

Because `fordwardSequence()`'s divider check is the *first* thing evaluated
per sequence per pulse, off-beat pulses cost one integer increment and
comparison per sequence — see Performance Considerations.

### 4. Persistence format bump

Add one byte to the sequence header for `_clockDivider` (0 = "use legacy
default of 6" for old files, preserving backward compatibility without a
version-gated branch):

```
seqHeader (9 bytes): type, midiChannel, color, lengthPages, inputPort, port,
                      clockDivider, nNotes(u16 LE)
```

Bump the file format `version` byte in the `RTP0` header so
`BuitPersistenceManager::loadSequencerFromBinary` can distinguish old
8-byte headers (assume divider=6) from new 9-byte headers. `gen_pattern.py`
et al. in `midi_to_pattern/` need a corresponding `clock_divider=6` default
argument added to `write_sequence()`.

---

## Design Concerns (from prior discussion — must be addressed, not deferred)

### Concern 1: Note length/TTL semantics must stay tied to *musical* time, not master-pulse count

`NotesPlayer::decreaseTimeToLive()` must **not** start firing at the raw
24-PPQN pulse rate just because one lane wants a fast divider — that would
silently shrink every other lane's note durations by ~6x-8x versus what the
composer intended (a `length=4` 16th-note note would decay in under a
32nd-note instead of a quarter-note). Two viable approaches:

- **(a) Keep TTL decrement at the current 16th-note cadence** (`CLOCK_GRID`),
  regardless of how fast any individual lane steps. A fast lane's very short
  notes (e.g. `length=1` on a 1/24 divider) round up to the TTL grid's
  resolution — acceptable, since sub-16th note lengths are already
  effectively "as short as possible" perceptually on real hardware.
- **(b) Move TTL decrement to be per-note, keyed to the real elapsed pulses
  since note-on**, independent of any lane's stepping divider. More
  correct, more invasive (touches `RTPEventNotePlus`/`NotesPlayer`'s ringing
  note bookkeeping to store pulse-based deadlines instead of a decrementing
  counter ticked externally).

Recommendation: ship **(a)** first — it's a one-line change (leave
`decreaseTimeToLive()`'s call site gated behind `CLOCK_GRID` instead of
moving it to the raw-pulse call), fully backward compatible, and only
"wrong" for lanes deliberately running faster than 16th notes with long
`length` values, which is a narrow, self-inflicted edge case composers can
route around empirically.

### Concern 2: Don't do full per-lane work on every raw pulse

The divider check in `fordwardSequence()` (Section 2) must be the *first*
line evaluated, before any of `playCurrentEventNote()`'s heavier logic
(chord lookups, `_notesPlayer.queueNote()`, MIDI dispatch). This keeps the
"not my turn" cost at one increment + one comparison per sequence per pulse
— confirmed cheap based on the existing `playCurrentEventNote()`
implementations (`MonoSequence`/`BassSequence`/`PolySequence`/`DrumSequence`,
all just pointer arithmetic + an `eventState()` guard). Actual MIDI I/O
(`RTPEventNotePlus::playNoteOn/Off` → `MidiRouter::route`) only fires when a
lane's own divider genuinely triggers a step — total MIDI traffic is
unchanged by the polling-resolution increase.

### Concern 3: UI/display callbacks stay at their current cadence

`GRID_TICK` (drives `syncLiveTrellis()`, screen updates) should **not** move
to the raw-pulse rate — there's no musical need for LED/display refresh
faster than 16th notes, and that code path (`setTrellisButtonColor`,
`printToScreen`) is more expensive per call than the sequence-stepping path.
Section 3's `gridClockUp` sketch keeps `GRID_TICK` gated at `CLOCK_GRID`
deliberately.

---

## Migration / Backward Compatibility

- Old pattern files (8-byte sequence headers) load with every lane defaulting
  to `_clockDivider = 6` — byte-for-byte identical playback to today.
- `gen_pattern.py`/`gen_pattern_dark.py`/`gen_pattern_full.py` in
  `midi_to_pattern/` need `write_sequence()` updated to accept and emit the
  new `clockDivider` byte (default `6`) once this ships, so future
  hand-composed patterns can opt individual lanes into faster/slower grids.

## Open Questions / Follow-ups

- UI: how does a user select a lane's divider on-device (which control
  surface page, what value range is exposed vs. the full raw-pulse
  integer)? Out of scope for this plan — needs its own UX pass.
- Should the enum eventually grow to include dotted-note / hemiola values
  (36, 18 pulses) as an explicit "advanced" tier, now that the safe default
  set is settled? Deliberately deferred, see Section 1.
- Concern 1(b) (fully correct per-note TTL independent of lane divider) is
  deferred; revisit if 1(a)'s rounding proves musically unsatisfying in
  practice.

## Status

Plan only — no implementation started. Builds directly on the existing
`HarmonySequence` divider precedent and the raw-24-PPQN clock generator
already in `src/Helpers/RTPClockGenerator.cpp`; no new clock hardware/timer
work required.
