#include "RTPThreeAxisVL.hpp"
#include "RTPMainUnit.hpp"
#include "ReMap.hpp"

RTPThreeAxisVL::RTPThreeAxisVL(){
    _maxLimitReading = DEFAULT_MAX_READING;
    _minLimitReading = DEFAULT_MIN_READING;
}

void RTPThreeAxisVL::initSetup(){
    pinMode(SENSOR_1_PIN, OUTPUT);
    pinMode(SENSOR_2_PIN, OUTPUT);
    pinMode(SENSOR_3_PIN, OUTPUT);
    digitalWrite(SENSOR_1_PIN, LOW);
    digitalWrite(SENSOR_2_PIN, LOW);
    digitalWrite(SENSOR_3_PIN, LOW);

    delay(5);
    sensor1.setBus(&Wire1);
    sensor2.setBus(&Wire1);
    sensor3.setBus(&Wire1);
    pinMode(SENSOR_1_PIN, INPUT);
    delay(15);
    sensor1.init(true);
    delay(10);
    sensor1.setAddress((uint8_t)22);

    //SENSOR 2
    pinMode(SENSOR_2_PIN, INPUT);
    delay(15);
    sensor2.init(true);
    delay(10);
    sensor2.setAddress((uint8_t)25);

    //SENSOR 3
    pinMode(SENSOR_3_PIN, INPUT);
    delay(15);
    sensor3.init(true);
    delay(10);
    sensor3.setAddress((uint8_t)28);

    sensor1.setTimeout(50);
    sensor2.setTimeout(50);
    sensor3.setTimeout(50);
}

void RTPThreeAxisVL::startContinuous(){
    sensor1.startContinuous();
    sensor2.startContinuous();
    sensor3.startContinuous();
}

void RTPThreeAxisVL::stopContinuous(){
    sensor1.stopContinuous();
    sensor2.stopContinuous();
    sensor3.stopContinuous();
}

void RTPThreeAxisVL::setMaxLimitReading(int maxReading){
    _maxLimitReading = maxReading;
}

int RTPThreeAxisVL::getMaxLimitReading(){
    return _maxLimitReading;
}

void RTPThreeAxisVL::setMinLimitReading(int minReading){
    _minLimitReading = minReading;
}

int RTPThreeAxisVL::getMinLimitReading(){
    return _minLimitReading;
}

ThreeReadings RTPThreeAxisVL::getThreeReadings(){
    ThreeReadings readings;
    readings.sensorReading1 = sensor1.readRangeContinuousMillimeters();
    readings.sensorReading2 = sensor2.readRangeContinuousMillimeters();
    readings.sensorReading3 = sensor3.readRangeContinuousMillimeters();
    return readings;
}

ThreeReadings RTPThreeAxisVL::getThreeCleanReadings(){
    ThreeReadings readings = getThreeReadings();
    readings.sensorReading1 = readings.sensorReading1 > _maxLimitReading ? -1 : readings.sensorReading1;
    readings.sensorReading1 = constrain(readings.sensorReading1, -1, _maxLimitReading);
    readings.sensorReading2 = readings.sensorReading2 > _maxLimitReading ? -1 : readings.sensorReading2;
    readings.sensorReading2 = constrain(readings.sensorReading2, -1, _maxLimitReading);
    readings.sensorReading3 = readings.sensorReading3 > _maxLimitReading ? -1 : readings.sensorReading3;
    readings.sensorReading3 = constrain(readings.sensorReading3, -1, _maxLimitReading);
    return readings;
}

void RTPThreeAxisVL::callbackThreeAxisChanged(void (*userFunc) (ControlCommand callbackCommand)){
    ThreeReadings readings = getThreeCleanReadings();
    if(readings.sensorReading1 != lastReadings.sensorReading1 && readings.sensorReading1 != -1){
        ControlCommand command;
        command.controlType = THREE_AXIS;
        command.commandType = CHANGE_LEFT;
        command.value = constrain(remap(readings.sensorReading1, _minLimitReading, _maxLimitReading, 0, 127), 0, 127);
        userFunc(command);
    }
    if(readings.sensorReading2 != lastReadings.sensorReading2 && readings.sensorReading2 != -1){
        ControlCommand command;
        command.controlType = THREE_AXIS;
        command.commandType = CHANGE_CENTER;
        command.value = constrain(remap(readings.sensorReading2, _minLimitReading, _maxLimitReading, 0, 127), 0, 127);
        userFunc(command);
    }
    if(readings.sensorReading3 != lastReadings.sensorReading3 && readings.sensorReading3 != -1){
        ControlCommand command;
        command.controlType = THREE_AXIS;
        command.commandType = CHANGE_RIGHT;
        command.value = constrain(remap(readings.sensorReading3, _minLimitReading, _maxLimitReading, 0, 127), 0, 127);
        userFunc(command);
    }
    lastReadings = readings;
}


void RTPThreeAxisVL::callbackThreeAxisChanged(RTPMainUnit* mainClass){
    ThreeReadings readings = getThreeCleanReadings();
    if(readings.sensorReading1 != lastReadings.sensorReading1 && readings.sensorReading1 != -1){
        ControlCommand command;
        command.controlType = THREE_AXIS;
        command.commandType = CHANGE_LEFT;
        command.value = constrain(remap(readings.sensorReading1, _minLimitReading, _maxLimitReading, 0, 127), 0, 127);
        mainClass->actOnControlsCallback(command);
    }
    if(readings.sensorReading2 != lastReadings.sensorReading2 && readings.sensorReading2 != -1){
        ControlCommand command;
        command.controlType = THREE_AXIS;
        command.commandType = CHANGE_CENTER;
        command.value = constrain(remap(readings.sensorReading2, _minLimitReading, _maxLimitReading, 0, 127), 0, 127);
        mainClass->actOnControlsCallback(command);
    }
    if(readings.sensorReading3 != lastReadings.sensorReading3 && readings.sensorReading3 != -1){
        ControlCommand command;
        command.controlType = THREE_AXIS;
        command.commandType = CHANGE_RIGHT;
        command.value = constrain(remap(readings.sensorReading3, _minLimitReading, _maxLimitReading, 0, 127), 0, 127);
        mainClass->actOnControlsCallback(command);
    }
    lastReadings = readings;
}