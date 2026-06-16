# RTPBuit

A modular music sequencer firmware for the **Teensy 4.1** platform, built with the Arduino framework using PlatformIO.

---

## Hardware

| Component | Part |
|---|---|
| MCU | Teensy 4.1 |
| Display | OLED via U8g2 |
| Button grid | Adafruit NeoTrellis |
| Sensors | VL53L0X three-axis sensor |
| MIDI | USB MIDI + Serial1 (5-pin DIN) |

---

## Build

```bash
# Normal firmware build
platformio run --environment teensy41

# Upload
platformio run --environment teensy41 --target upload

# Clean
platformio run --environment teensy41 --target clean

# Unit tests (flashes test firmware, read results over Serial at 115200 baud)
platformio run --environment teensy41_test --target upload
```

---

## Project Structure

```
include/
├── Config/          # Centralised constants (MusicConfig, HardwareConfig, UiConfig)
├── DI/              # Dependency injection container (ServiceContainer)
├── Error/           # Error types, Result<T>, Logger, ErrorHandler
├── Interfaces/      # Pure abstract interfaces (IMidiOutput, IDisplay, IInputDevice…)
├── Hardware/
│   └── Implementations/   # Concrete hardware drivers (OledDisplay, TeensyMidiOutput…)
├── Managers/        # Focused manager classes (DeviceManager, DisplayManager, …)
├── Sequencer/       # Sequencer logic
├── StateMachine/    # State machine (BassSequence, …)
└── Validation/      # Stateless validation utilities (MidiValidator, InputValidator, …)

src/
├── Helpers/         # Legacy helpers (retained for compatibility)
├── Hardware/        # Hardware implementation sources
├── Managers/        # Manager sources
└── Sequencer/       # Sequencer sources

test/
├── Framework/       # Lightweight assertion framework (Assert.hpp)
├── Mocks/           # Mock hardware implementations (MockMidiOutput, MockDisplay)
├── Tests/Unit/      # Unit test suites (TestValidation, …)
└── test_main.cpp    # Test entry point (setup/loop for Teensy)
```

---

## Architecture

The codebase follows SOLID principles:

- **SRP** — Each manager class has exactly one responsibility (`DisplayManager`, `InputManager`, `TransportManager`, `DeviceManager`).
- **OCP** — New hardware is added by implementing an interface, not modifying existing code.
- **LSP** — All concrete implementations are substitutable through their interfaces.
- **ISP** — Interfaces are narrow (`IMidiOutput`, `IDisplay`, `IButtonMatrix`, `IRotaryEncoder`, `IThreeAxisSensor`).
- **DIP** — Managers receive their dependencies via constructor injection (`std::shared_ptr<IFoo>`).

### Error Handling

All fallible public methods return `Result<T>`:

```cpp
Result<void> TransportManager::initialize();
Result<void> InputManager::initialize();
```

Use `Result<void>::Ok()` for success and `Result<void>::failure(code, severity, message)` for errors. The `ErrorHandler` provides centralised logging and recovery.

### Validation

Stateless utilities in `include/Validation/`:

```cpp
MidiValidator::isValidNote(note);       // 0–127
MidiValidator::isValidChannel(channel); // 1–16
InputValidator::isValidBPM(bpm);        // 40–240
RangeChecker::clamp(value, min, max);
```

---

## Running Tests

The test firmware replaces `setup()`/`loop()` and streams results over Serial:

```
=== Validation | PASS: 9  FAIL: 0
```

Add new test suites in `test/Tests/Unit/`, call `runAll()` from `test_main.cpp`.

---

## Dependencies

Managed by PlatformIO (`platformio.ini`):

| Library | Purpose |
|---|---|
| olikraus/U8g2 | OLED display |
| pololu/VL53L0X | Distance sensor |
| Adafruit NeoPixel / Trellis / seesaw | Button grid |
| bblanchon/ArduinoJson | Persistence (JSON) |
| Nasker/RTPLib | Core sequencer library |
| PaulStoffregen/Encoder | Rotary encoder |
