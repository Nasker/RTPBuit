#pragma once

#include <SD.h>
#include <SPI.h>

const int chipSelect = BUILTIN_SDCARD;

void initBuitSD();
bool writeToFile(String fileName, String data);
bool readFromFile(String fileName, String &data);

