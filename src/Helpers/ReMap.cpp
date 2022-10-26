#include "ReMap.hpp"

int remap (int value, int min, int max, int newMin, int newMax){
    float remappedValue = (float)(value - min) / (float)(max - min);
    float newValue = remappedValue * (newMax - newMin) + newMin;
    return (int)newValue;
}