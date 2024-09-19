#ifndef IMAGE_EDITING_H
#define IMAGE_EDITING_H

#include <Arduino.h>

// tweak-able values
#define MIN_RED_FACTOR 1.6
#define MIN_RED_BRIGHTNESS 70

// image characteristics
#define IMAGE_HEIGHT 240
#define IMAGE_WIDTH 32

struct pixel
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;

    pixel & operator= (const pixel& p_rhs)
    {
        if(this != &p_rhs)
        {
            this->red = p_rhs.red;
            this->green = p_rhs.green;
            this->blue = p_rhs.blue;
        }
        return *this;
    }
};

#endif