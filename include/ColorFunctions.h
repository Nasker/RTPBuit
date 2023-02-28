#pragma once

#include <Arduino.h>
#include "constants.h"

int colorScaler(int color, int scalar, int max);
int colorMapper(int colorIndex);

const int colorWheel[N_COLORS]={0xFF0000,
                                     0xFF2F00,
                                     0xFF5F00,
                                     0xFF8F00,
                                     0xFFBF00,
                                     0xFFEF00,
                                     0xDFFF00,
                                     0xAFFF00,
                                     0x7FFF00,
                                     0x4FFF00,
                                     0x1FFF00,
                                     0x00FF0F,
                                     0x00FF3F,
                                     0x00FF6F,
                                     0x00FF9F,
                                     0x00FFCF,
                                     0x00FFFF,
                                     0x00CFFF,
                                     0x009FFF,
                                     0x006FFF,
                                     0x003FFF,
                                     0x000FFF,
                                     0x1F00FF,
                                     0x4F00FF,
                                     0x7F00FF,
                                     0xAF00FF,
                                     0xDF00FF,
                                     0xFF00EF,
                                     0xFF00BF,
                                     0xFF008F,
                                     0xFF005F,
                                     0xFF002F};