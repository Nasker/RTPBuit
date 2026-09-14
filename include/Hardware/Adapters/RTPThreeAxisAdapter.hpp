#pragma once

#include "Interfaces/IInputDevice.hpp"
#include "Devices/RTPThreeAxisVL.hpp"
#include <functional>

/**
 * @brief Adapter to make RTPThreeAxisVL compatible with IThreeAxisSensor interface
 * 
 * This adapter wraps the existing RTPThreeAxisVL class and implements the IThreeAxisSensor
 * interface, allowing it to be used with the new decomposed managers.
 */
class RTPThreeAxisAdapter : public IThreeAxisSensor {
private:
    RTPThreeAxisVL& _sensor;
    bool _ready;
    std::function<void(int, int, int)> _changeCallback;

public:
    /**
     * @brief Constructor
     * @param sensor Reference to existing RTPThreeAxisVL instance
     */
    explicit RTPThreeAxisAdapter(RTPThreeAxisVL& sensor) 
        : _sensor(sensor), _ready(false) {}

    bool initialize() override {
        // RTPThreeAxisVL is initialized via initSetup()/startContinuous() in
        // RTPMainUnit::begin(). Re-running its XSHUT/addressing sequence here
        // would race with continuous ranging and brick the sensors (65535s).
        _ready = true;
        return true;
    }

    void update() override {
        // RTPThreeAxisVL is updated via callbackThreeAxisChanged in RTPMainUnit
    }

    bool isReady() const override {
        return _ready;
    }

    void setChangeCallback(std::function<void(int, int, int)> callback) override {
        _changeCallback = callback;
    }

    ThreeReadings getCurrentReadings() const override {
        return const_cast<RTPThreeAxisVL&>(_sensor).getThreeReadings();
    }

    ThreeReadings getCleanReadings() const override {
        return const_cast<RTPThreeAxisVL&>(_sensor).getThreeCleanReadings();
    }

    void setMinReading(int minReading) override {
        _sensor.setMinLimitReading(minReading);
    }

    void setMaxReading(int maxReading) override {
        _sensor.setMaxLimitReading(maxReading);
    }
};
