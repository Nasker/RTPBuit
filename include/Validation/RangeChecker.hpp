#pragma once

#include <Arduino.h>

/**
 * @brief Generic range checking utilities
 * 
 * Stateless helpers for boundary validation across all subsystems.
 */
namespace RangeChecker {

    template<typename T>
    constexpr bool inRange(T value, T minVal, T maxVal) {
        return value >= minVal && value <= maxVal;
    }

    template<typename T>
    constexpr bool inRangeExclusive(T value, T minVal, T maxVal) {
        return value > minVal && value < maxVal;
    }

    template<typename T>
    constexpr T clamp(T value, T minVal, T maxVal) {
        if (value < minVal) return minVal;
        if (value > maxVal) return maxVal;
        return value;
    }

    template<typename T>
    constexpr bool isValidIndex(T index, T containerSize) {
        return index >= 0 && index < containerSize;
    }

    template<typename T>
    constexpr bool isPositive(T value) {
        return value > 0;
    }

    template<typename T>
    constexpr bool isNonNegative(T value) {
        return value >= 0;
    }

}
