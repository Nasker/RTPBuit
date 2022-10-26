#include "RTPSDManager.hpp"

void initBuitSD(){  
    if (!SD.begin(chipSelect)) {
        Serial.println("initialization failed!");
        return;
    }
    Serial.println("initialization done.");
}

bool writeToFile(String fileName, String data){
    File file = SD.open(fileName.c_str(), FILE_WRITE);
    if (file)
        Serial.println("File opened");
    else {
        Serial.println("error opening file");
        return false;
    }
    Serial.println("Writing To File");
    file.print(data);
    file.close();
    Serial.println("done.");
    return true;
}

bool readFromFile(String fileName, String &data){
    File file = SD.open(fileName.c_str(), FILE_READ);
    if (file)
        Serial.println("File opened");
    else {
        Serial.println("error opening file");
        return false;
    }
    Serial.println("Reading from File");
    while (file.available())
        data += file.readStringUntil('\n');     //data += file.read();
    file.close();
    Serial.println("done.");
    return true;
}