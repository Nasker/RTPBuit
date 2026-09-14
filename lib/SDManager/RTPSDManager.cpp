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

bool fileExistsOnSD(const String& fileName) {
    return SD.exists(fileName.c_str());
}

bool readFromFile(String fileName, String &data) {
    File file = SD.open(fileName.c_str(), FILE_READ);
    if (!file) {
        Serial.println("Error opening file");
        return false;
    }
    
    Serial.println("Reading from file: " + fileName);
    data = ""; // Clear any existing data
    
    // Read the entire file as a string
    while (file.available()) {
        // Read as characters, not as integers
        data += (char)file.read();
    }
    
    file.close();
    Serial.println("File read complete");
    
    // Optional: Print first 100 chars to debug
    if (data.length() > 0) {
        Serial.printf("Data length: %d\n", data.length());
    }
    return true;
}

File openFileForRead(const String& fileName) {
    File file = SD.open(fileName.c_str(), FILE_READ);
    if (file) {
        Serial.println("Opened file stream: " + fileName);
    } else {
        Serial.println("Error opening file stream: " + fileName);
    }
    return file;
}

File openFileForWrite(const String& fileName) {
    File file = SD.open(fileName.c_str(), FILE_WRITE_BEGIN);
    if (file) {
        Serial.println("Opened file stream for write: " + fileName);
    } else {
        Serial.println("Error opening file stream for write: " + fileName);
    }
    return file;
}