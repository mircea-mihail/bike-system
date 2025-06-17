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

struct stop_chunk
{
    point top_left;
    point top_right;
    point right_top;
    point right_bottom;
    point bottom_right;
    point bottom_left;
    point left_bottom;
    point left_top;


    stop_chunk(point p_top_left, point p_top_right, point p_right_top, point p_right_bottom,
        point p_bottom_right, point p_bottom_left, point p_left_bottom, point p_left_top) 
        : top_left(p_top_left), top_right(p_top_right), right_top(p_right_top), right_bottom(p_right_bottom), 
        bottom_right(p_bottom_right), bottom_left(p_bottom_left), left_bottom(p_left_bottom), left_top(p_left_top) 
    {}

    stop_chunk() : top_left(point()), top_right(point()), right_top(point()), right_bottom(point()), 
        bottom_right(point()), bottom_left(point()), left_bottom(point()), left_top(point()) 
    {}
};

struct circle_chunk
{
    point top;
    point bottom;
    point left;
    point right;

    circle_chunk(point p_top, point p_bottom, point p_left, point p_right)
    {
        top = p_top;
        bottom = p_bottom;
        left = p_left;
        right = p_right;
    }

    circle_chunk()
    {
        top = point();
        bottom = point();
        left = point();
        right = point();
    }
};

struct crossing_chunk
{
    point left;
    point right;
    point top;

    crossing_chunk(point p_top, point p_left, point p_right)
    {
        top = p_top;
        left = p_left;
        right = p_right;
    }

    crossing_chunk()
    {
        top = point();
        left = point();
        right = point();
    }
};

///////////////////////////////// tweak-able constants
#define PRINT_STATS
// #define IN_RASPI

#define LOG_PIC_PATH "/home/mircea/.bike-sys-data"
#define STOP_PATH "./templates/stop_100.png"
#define NO_BIKES_PATH "./templates/no_bikes_100.png"
#define CROSSING_PATH "./templates/crossing_100.png"
#define WRONG_WAY_PATH "./templates/wrong_way_100.png"

// ------------------------------- IMAGE
#define IMAGE_HEIGHT (960)
#define IMAGE_WIDTH (1280)

#define STOP_TEMPLATE_WIDTH 100
#define STOP_TEMPLATE_HEIGHT 101
#define NO_BIKES_TEMPLATE_WIDTH 100
#define NO_BIKES_TEMPLATE_HEIGHT 100


#define STOP_POSITION 0
#define NO_BIKES_POSITION 1
#define CROSSING_POSITION 2
#define WRONG_WAY_POSITION 3

#define SAVE_IMG_RATIO 1
#define SAVED_IMG_HEIGHT (IMAGE_HEIGHT / SAVE_IMG_RATIO) 
#define SAVED_IMG_WIDTH (IMAGE_WIDTH / SAVE_IMG_RATIO)

// ------------------------------- CHUNK
#define MIN_CHUNK_SIZE 10
#define MIN_BOUNDING_BOX_AREA 200
#define MIN_CHUNK_SCORE 0.70
#define MAX_CLUMPING_VARIANCE 5 

// ------------------------------- TIMING
#define CONTROL_PIC_INTERVAL_MS 10000 // 10 s

// ------------------------------- DETECTION RELATED
#define MIN_MAYBE_IDX 3
#define FAILED_TO_ROTATE_VAL 361
#define DRAW_LINE_THICKNESS 3   

// ------------------------------- GIVE WAY RED BORDER   
// #define THICK_GW_BORDER_PER_CENT 0.376
#define THICK_GW_BORDER_PER_CENT 0.4
#define THIN_GW_BORDER_PER_CENT 0.68
// for thin german give way:
// #define THIN_GW_BORDER_PER_CENT 0.88
#define MIN_TRIANGLE_ANGLE 40
#define OCTOGON_ANGLE_THRESHOLD 20
#define IDEAL_OCTOGON_ANGLE 135

#define IDEAL_SQUARE_ANGLE 90
#define SQUARE_ANGLE_THRESHOLD 20

#define RED_OUTSIDE_GW_THRESHOLD 0.4
#define RED_OUTSIDE_STOP_THRESHOLD 0.3
#define RED_ABOVE_SIGN (1/4)

// ------------------------------- RED COLOR DETECTION
#define MAX_HUE 179
#define VALUE_DELIMITER 90 
#define VALUE_DELIMITER_OFFSET 10

#define HUE_POSITIVE_BRIGHT_OFFSET 8
#define HUE_NEGATIVE_BRIGHT_OFFSET 10
#define HUE_POSITIVE_DARK_OFFSET 10
#define HUE_NEGATIVE_DARK_OFFSET 20

#define BRIGHT_MIN_RED_SATURATION 100

#define DARK_MIN_RED_SATURATION 140
#define DARK_MIN_RED_VALUE 30

#define MAX_SATURATION 255
#define MAX_VALUE 255
#define PX_MAX_VAL 255

// ------------------------------- WHITE COLOR DETECTION 
// before min val 40
#define MIN_WHITE_VALUE 60
#define MAX_WHITE_SATURATION 130

#define MAX_BLACK_VALUE 100

#define RED 2
#define GREEN 1
#define BLUE 0

#define COLOR_THRESHOLD 128

///////////////////////////////// rarely modified constants
// ------------------------------- templates 

#endif
