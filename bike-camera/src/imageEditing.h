#ifndef IMAGE_EDITING_H
#define IMAGE_EDITING_H

#include <Arduino.h>
#include <pictureTaking.h>

#include "imageDefines.h"

// tweak-able values
#define MIN_RED_FACTOR 1.6
#define MIN_RED_BRIGHTNESS 70

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

/// @brief checks if a pixel is red
/// @param p_px the rgb pixel
/// @return if the pixel is red or not
bool is_red(pixel p_px);

/// @brief proof of concept function that returns true
/// when detecting a red pixel in the image
/// @return true if finds any red pixel, false otherwise
bool findRedInPic();

/// @brief conversion function that takes 2 bytes of a rgb565 px 
/// and returns an rgb 888 pixel struct
/// @param p_pixel16 rgb565 pixel
/// @return rgb 888 pixel struct
pixel get_rgb888_from_rgb565(uint16_t p_pixel16);

#endif