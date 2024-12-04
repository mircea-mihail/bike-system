#ifndef UTILITY_H
#define UTILITY_H

#include <cstdint>
#include "constants.h"

bool is_red(uint8_t p_hue, uint8_t p_saturation, uint8_t p_value);

bool is_red(cv::Vec3b hsv_px);

bool is_white(uint8_t p_hue, uint8_t p_saturation, uint8_t p_value);

bool is_white(cv::Vec3b hsv_px);

double point_area(point p_point1, point p_point2, point p_point3);

double chunk_area(give_way_chunk p_chunk);

point scale_point_towards_centroid(point p_point, point p_centroid, double p_scale_factor);

give_way_chunk get_inner_triangle_chunk(give_way_chunk p_chunk, double p_scale);

// gets what side the point is on in relation to a line
float get_side(point p_point, point p_vertex_p1, point p_vertex_p2);

bool point_in_triangle(point p_point, point p_point_tr_1, point p_point_tr_2, point p_point_tr_3);

// try to find a condition to filter based on shape
bool has_small_angle(give_way_chunk p_chunk);

bool check_color_variance(uint8_t p_red_1, uint8_t p_green_1, uint8_t p_blue_1, uint8_t p_red_2, uint8_t p_green_2, uint8_t p_blue_2);

bool check_color_variance(cv::Vec3b p_hsv_px_1, cv::Vec3b p_hsv_px_2);

#endif