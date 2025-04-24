#include "RTPSDManager.hpp"

void initBuitSD(){  
    if (!SD.begin(chipSelect)) {
        Serial.println("initialization failed!");
        return;
    }
    Serial.println("initialization done.");
}

bool writeToFile(String fileName, String data){
    // Use FILE_WRITE_BEGIN to truncate the file if it exists or create a new one
    File file = SD.open(fileName.c_str(), FILE_WRITE_BEGIN);
    if (file)
        Serial.println("File opened");
    else {
        Serial.println("error opening file");
        return false;
    }
    Serial.println("Writing To File");
    Serial.println(data);
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