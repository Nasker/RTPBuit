# RTPBuit

## Overview
RTPBuit is a custom MIDI sequencer firmware designed for the Teensy 4.1 platform. It combines hardware control surfaces, MIDI sequencing capabilities, and a state machine architecture to create a versatile electronic music instrument. The project features real-time note recording, multiple sequence types, and an intuitive user interface with OLED display and NeoTrellis grid feedback.

## Hardware
- **Microcontroller**: Teensy 4.1
- **Display**: OLED screen for visual feedback
- **Control Surface**: Adafruit NeoTrellis grid (16 buttons with RGB LEDs)
- **Input Controls**: Rotary encoder with push button, VL53L0X proximity sensor
- **Connectivity**: MIDI I/O via USB and hardware MIDI ports

## Features
- **Multi-track Sequencing**: Support for multiple sequence types:
  - Drum sequences
  - Bass sequences
  - Monophonic lead sequences
  - Polyphonic sequences
  - Control sequences
  - Harmony sequences
- **Real-time Recording**: Record MIDI notes in real-time with quantization
- **Scene Management**: Organize sequences into scenes for live performance
- **State Machine Architecture**: Clean separation of concerns with different operational states
- **Persistence**: Save and load sequences to/from SD card
- **Visual Feedback**: OLED display and RGB button feedback
- **MIDI Routing**: Process incoming MIDI and route to appropriate sequences

## Project Structure

### Core Components
- **State Machine**: Manages application state and user interactions
- **Sequencer**: Handles timing, playback, and recording of musical events
- **Device Managers**: Interface with hardware components
- **Persistence**: JSON-based storage of sequences and settings

### Key Classes
- `RTPMainUnit`: Central coordinator for the application
- `BuitStateMachine`: Manages state transitions and user input
- `RTPSequencer`: Handles sequence playback and recording
- `BuitDevicesManager`: Manages hardware interfaces
- `RTPEventNoteSequence`: Base class for different sequence types

## Building and Flashing

### Prerequisites
- PlatformIO
- Arduino framework
- Teensy core libraries

### Dependencies
- U8g2 (OLED display)
- Adafruit NeoTrellis
- Adafruit NeoPixel
- ArduinoJson
- MIDI Library
- Audio (Teensy Audio Library)

### Build Commands
```bash
# Build the project
pio run

# Upload to Teensy 4.1
pio run --target upload --environment teensy41
```

## Usage

The RTPBuit interface is organized into different states:

- **Scene Select**: Choose between different scenes
- **Sequence Select**: Select a sequence within a scene
- **Sequence Edit**: Edit notes within a sequence
- **Transport**: Control playback (play/stop/etc.)
- **Settings**: Configure sequence parameters

Navigation is primarily done through the rotary encoder, buttons, and proximity sensor.

### Recording Notes
1. Enter Sequence Edit state
2. Toggle recording mode
3. Play notes via MIDI input to record them in real-time

## Development

### Branches
- `main`: Stable release branch
- `feature/*`: Feature development branches

### Coding Conventions
- Minimal comments, self-documenting code
- Consistent state transitions with proper UI updates
- Memory-efficient JSON field names (shortened to single characters)

## Companion Projects
- **SequenceVisualizer**: Web application for visualizing and editing sequences

## License
All rights reserved. This project is proprietary software.

## Credits
Developed by Real-Time Porridge (RTP)
