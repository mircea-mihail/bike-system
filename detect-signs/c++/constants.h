#ifndef CONSTANTS_H
#define CONSTANTS_H

///////////////////////////////// useful structs
struct point
{
    float x;
    float y;
};

struct give_way_chunk
{
    point top_left_point;
    point top_right_point;
    point bottom_point;
};

///////////////////////////////// tweak-able constants
// ------------------------------- CHUNK
#define MIN_CHUNK_SIZE 10
#define MIN_CHUNK_SCORE 0.60
#define MAX_CLUMPING_VARIANCE 5 

// ------------------------------- GIVE WAY RED BORDER   
#define THICK_GW_BORDER_PER_CENT 0.356
#define THIN_GW_BORDER_PER_CENT 0.72
#define MIN_TRIANGLE_ANGLE 40
#define RED_OUTSIDE_GW_THRESHOLD 0.4
#define RED_ABOVE_SIGN (1/4)

// ------------------------------- RED COLOR DETECTION
#define MAX_HUE 179
#define VALUE_DELIMITER 110 
#define ALLOWED_RED_HUE_OFFSET 8

#define BRIGHT_MIN_RED_SATURATION 100

#define DARK_MIN_RED_SATURATION 80
#define DARK_MIN_RED_VALUE 50

// ------------------------------- WHITE COLOR DETECTION 
#define MIN_WHITE_BRIGHTNESS 40
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