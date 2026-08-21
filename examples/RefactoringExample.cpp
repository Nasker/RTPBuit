/**
 * @file RefactoringExample.cpp
 * @brief Example showing the new architecture with interfaces and dependency injection
 * 
 * This file demonstrates how the refactored system works compared to the old hardcoded approach.
 */

#include "Interfaces/IMidiOutput.hpp"
#include "Interfaces/IDisplay.hpp"
#include "Hardware/Implementations/TeensyMidiOutput.hpp"
#include "Hardware/Implementations/OledDisplay.hpp"
#include "DI/ServiceContainer.hpp"
#include "Error/ErrorHandler.hpp"
#include "Config/HardwareConfig.hpp"
#include "Config/MusicConfig.hpp"

// Example of old hardcoded approach (what we're refactoring from)
void oldHardcodedApproach() {
    // Old way - hardcoded dependencies
    usbMIDI.sendNoteOn(60, 90, 1);  // C4 on channel 1
    Serial1.write(0x90);           // MIDI status byte
    Serial1.write(60);             // Note
    Serial1.write(90);             // Velocity
}

// Example of new interface-based approach
void newInterfaceApproach(std::shared_ptr<IMidiOutput> midiOutput) {
    // New way - dependency injected interface
    midiOutput->sendNoteOn(60, 90, 1);  // C4 on channel 1
    // Automatically handles USB and serial MIDI based on configuration
}

// Example of dependency injection setup
class ExampleApplication {
private:
    std::shared_ptr<IMidiOutput> _midiOutput;
    std::shared_ptr<IDisplay> _display;
    ServiceContainer _container;

public:
    bool initialize() {
        // Register services
        _container.registerService<IMidiOutput, TeensyMidiOutput>(ServiceLifetime::Singleton);
        _container.registerService<IDisplay, OledDisplay>(ServiceLifetime::Singleton);
        
        // Resolve services
        _midiOutput = _container.resolve<IMidiOutput>();
        _display = _container.resolve<IDisplay>();
        
        if (!_midiOutput || !_display) {
            ErrorHandler::handleError(ErrorCode::HardwareInitFailed, ErrorSeverity::Critical,
                                     "Failed to resolve required services", "ExampleApplication");
            return false;
        }
        
        // Initialize hardware
        if (!_display->initialize()) {
            return false;
        }
        
        _display->printFourLines("RTPBuit", "Refactored", "Architecture", "Ready!");
        
        return true;
    }
    
    void playDemoNotes() {
        if (!_midiOutput) return;
        
        // Play a C major arpeggio
        uint8_t notes[] = {60, 64, 67, 72};  // C4, E4, G4, C5
        
        for (int i = 0; i < 4; i++) {
            _midiOutput->sendNoteOn(notes[i], MusicConfig::Velocity::DEFAULT_VELOCITY, 1);
            delay(200);
            _midiOutput->sendNoteOff(notes[i], 0, 1);
        }
    }
    
    void demonstrateErrorHandling() {
        // Example of error handling with the new system
        try {
            // This will be caught and logged by our error handling system
            _midiOutput->sendNoteOn(200, 200, 20);  // Invalid parameters
        } catch (...) {
            // Exceptions are disabled, but our validation catches this
        }
        
        // Log a custom error
        ErrorHandler::handleError(ErrorCode::InvalidNote, ErrorSeverity::Warning,
                                 "Demo error for testing", "ExampleApplication");
    }
};

// Example of configuration usage
void demonstrateConfiguration() {
    // Old way - magic numbers
    // int someValue = 128;  // What does 128 mean?
    
    // New way - meaningful configuration
    int sensorRange = MusicConfig::ThreeAxis::SENSOR_READING_RANGE;
    int maxVelocity = MusicConfig::Velocity::MAX_VELOCITY;
    int oledWidth = HardwareConfig::Display::SCREEN_WIDTH;
    
    // Configuration is self-documenting and centralized
}

// Example of how to migrate existing code
class BassSequenceRefactored {
private:
    std::shared_ptr<IMidiOutput> _midiOutput;  // Injected instead of hardcoded
    
public:
    BassSequenceRefactored(std::shared_ptr<IMidiOutput> midiOutput) 
        : _midiOutput(midiOutput) {}
    
    // Old method signature
    // void playLiveNoteOn(uint8_t rootNote, uint8_t velocity, uint8_t chordType) {
    //     usbMIDI.sendNoteOn(rootNote, velocity, getMidiChannel());
    //     Serial1.write(0x90 | ((getMidiChannel() - 1) & 0x0F));
    //     Serial1.write(rootNote & 0x7F);
    //     Serial1.write(velocity & 0x7F);
    // }
    
    // New method signature with error handling
    void playLiveNoteOn(uint8_t rootNote, uint8_t velocity, uint8_t chordType) {
        if (!_midiOutput) {
            ErrorHandler::handleError(ErrorCode::InvalidState, ErrorSeverity::Error,
                                     "MIDI output not available", "BassSequenceRefactored");
            return;
        }
        
        _midiOutput->sendNoteOn(rootNote, velocity, getMidiChannel());
    }
    
private:
    uint8_t getMidiChannel() { return 1; }  // Simplified for example
};

// Main function showing the setup
void setupRefactoredSystem() {
    // Initialize error handling
    ErrorHandler::setDefaultPolicy(ErrorPolicy::LogAndRecover);
    
    // Create and initialize application
    ExampleApplication app;
    
    if (!app.initialize()) {
        ErrorHandler::handleError(ErrorCode::HardwareInitFailed, ErrorSeverity::Critical,
                                 "Failed to initialize application", "setupRefactoredSystem");
        return;
    }
    
    // Run demo
    app.playDemoNotes();
    app.demonstrateErrorHandling();
    
    // Demonstrate configuration
    demonstrateConfiguration();
    
    LOG_INFO_CTX("Refactored system demo completed successfully", "setupRefactoredSystem");
}
