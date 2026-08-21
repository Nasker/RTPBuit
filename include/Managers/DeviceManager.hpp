#pragma once

#include "DisplayManager.hpp"
#include "InputManager.hpp"
#include "TransportManager.hpp"
#include "Interfaces/IMidiOutput.hpp"
#include "Error/Result.hpp"
#include <Arduino.h>
#include <memory>
#include <functional>

/**
 * @brief Device coordination manager
 * 
 * Replaces the BuitDevicesManager god object by coordinating focused managers.
 * Follows Single Responsibility Principle by delegating to specialized managers.
 */
class DeviceManager {
private:
    std::unique_ptr<DisplayManager> _displayManager;
    std::unique_ptr<InputManager> _inputManager;
    std::unique_ptr<TransportManager> _transportManager;
    std::shared_ptr<IMidiOutput> _midiOutput;
    
    bool _ready;
    uint32_t _lastErrorTime;

public:
    /**
     * @brief Constructor
     * @param display Display interface
     * @param buttonMatrix Button matrix interface
     * @param rotaryEncoder Rotary encoder interface
     * @param threeAxisSensor Three-axis sensor interface
     * @param clockGenerator Clock generator interface
     * @param sequencer Sequencer interface
     * @param midiOutput MIDI output interface
     */
    DeviceManager(std::shared_ptr<IDisplay> display,
                  std::shared_ptr<IButtonMatrix> buttonMatrix,
                  std::shared_ptr<IRotaryEncoder> rotaryEncoder,
                  std::shared_ptr<IThreeAxisSensor> threeAxisSensor,
                  std::shared_ptr<IClockGenerator> clockGenerator,
                  std::shared_ptr<ISequencer> sequencer,
                  std::shared_ptr<IMidiOutput> midiOutput);

    /**
     * @brief Initialize the device manager
     * @return true if initialization successful
     */
    Result<void> initialize();

    /**
     * @brief Check if device manager is ready
     * @return true if ready
     */
    bool isReady() const;

    // Update Operations
    void update();
    void poll();

    // Manager Access (for delegation)
    DisplayManager* getDisplayManager() const { return _displayManager.get(); }
    InputManager* getInputManager() const { return _inputManager.get(); }
    TransportManager* getTransportManager() const { return _transportManager.get(); }

    // Convenience Methods (delegated to managers)
    
    // Display convenience methods
    void printFourLines(const String& line1, const String& line2, const String& line3, const String& line4);
    void displayTransportState();
    void displayError(const String& errorMessage);
    void showIntroAnimation();

    // Input convenience methods  
    void setButtonPressCallback(std::function<void(uint8_t)> callback);
    void setRotaryCallback(std::function<void(bool)> callback);
    void setThreeAxisCallback(std::function<void(int, int, int)> callback);
    void writeTransportPage();

    // Transport convenience methods
    void play();
    void stop();
    void togglePlayback();
    void toggleRecording();
    void setBPM(float bpm);
    void tapTempo();

    // System Operations
    void reset();
    void shutdown();
    String getStatusString() const;

    // Diagnostics
    struct SystemStatus {
        bool displayReady;
        bool inputReady;
        bool transportReady;
        bool midiReady;
        bool overallReady;
        String lastError;
        uint32_t uptimeMs;
    };
    
    SystemStatus getSystemStatus() const;

private:
    void logError(const String& message);
    Result<void> initializeManagers();
    void setupEventRouting();
};
