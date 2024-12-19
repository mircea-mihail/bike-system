#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <opencv2/opencv.hpp> 
///////////////////////////////// useful structs
struct point
{
    double x;
    double y;

    point()
    {
        x = 0; 
        y = 0;
    }

    point(double p_x, double p_y)
    {
        this->x = p_x;
        this->y = p_y;
    }

    point &operator=(const point &rhs)
    {
        if(this == &rhs)
        {
            return *this;
        }
    
        this->x = rhs.x;
        this->y = rhs.y;

        return *this;
    }

    cv::Point get_cv_point()
    {
        return cv::Point(x, y);
    }

};

struct give_way_chunk
{
    point top_left;
    point top_right;
    point bottom;

    give_way_chunk(point p_top_left, point p_top_right, point p_bottom)
    {
        top_left = p_top_left;
        top_right = p_top_right;
        bottom = p_bottom;
    }

    give_way_chunk()
    {
        top_left = point();
        top_right = point();
        bottom = point();
    }
};

///////////////////////////////// tweak-able constants
#define PRINT_STATS

// ------------------------------- IMAGE
#define IMAGE_HEIGHT (1600)
#define IMAGE_WIDTH (1200)

// ------------------------------- CHUNK
#define MIN_CHUNK_SIZE 10
#define MIN_BOUNDING_BOX_AREA 50
#define MIN_CHUNK_SCORE 0.75
#define MAX_CLUMPING_VARIANCE 5 

// ------------------------------- GIVE WAY RED BORDER   
#define THICK_GW_BORDER_PER_CENT 0.356
#define THIN_GW_BORDER_PER_CENT 0.72
#define MIN_TRIANGLE_ANGLE 35
#define RED_OUTSIDE_GW_THRESHOLD 0.4
#define RED_ABOVE_SIGN (1/4)

// ------------------------------- RED COLOR DETECTION
#define MAX_HUE 179
#define VALUE_DELIMITER 110 
#define ALLOWED_RED_HUE_OFFSET 8

#define BRIGHT_MIN_RED_SATURATION 100

#define DARK_MIN_RED_SATURATION 80
#define DARK_MIN_RED_VALUE 50

#define MAX_SATURATION 255
#define MAX_VALUE 255

// ------------------------------- WHITE COLOR DETECTION 
// before min val 40
#define MIN_WHITE_VALUE 100
#define MAX_WHITE_SATURATION 130

///////////////////////////////// rarely modified constants
// ------------------------------- PIXLES 
#define BLUE 0
#define GREEN 1
#define RED 2

#define HUE 0
#define SATURATION 1
#define VALUE 2

#endif