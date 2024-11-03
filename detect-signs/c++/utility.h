#ifndef UTILITY_H
#define UTILITY_H

#include <cstdint>
#include "constants.h"

bool isRed(uint8_t p_hue, uint8_t p_saturation, uint8_t p_value)
{
    if (p_hue < ALLOWED_RED_HUE_OFFSET || p_hue > MAX_HUE - ALLOWED_RED_HUE_OFFSET)
    {
        // dark red:
        if (p_value < VALUE_DELIMITER)
        {
            if (p_value > DARK_MIN_RED_VALUE && p_saturation > DARK_MIN_RED_SATURATION)
            {
                return true;
            }
        }
        else
        {
            if (p_saturation > BRIGHT_MIN_RED_SATURATION)
            {
                return true;
            }
        }
    }
    return false;
}

#endif