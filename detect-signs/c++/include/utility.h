#ifndef UTILITY_H
#define UTILITY_H

#include <cstdint>

#include "constants.h"

/// @brief aplies hue saturation and value constraints to determine if a pixel is red
/// @param p_hue hue value
/// @param p_saturation saturation value
/// @param p_value value value
/// @return true if is red, false if not
bool is_red(uint8_t p_hue, uint8_t p_saturation, uint8_t p_value);

/// @brief aplies hue saturation and value constraints to determine if a pixel is red
/// @param hsv_px an array with hue on pos 0, saturation on pos 1 and value on pos 2
/// @return true if is red, false if not
bool is_red(cv::Vec3b p_hsv_px);

/// @brief aplies hue saturation and value constraints to determine if a pixel is white
/// @param p_hue hue value
/// @param p_saturation saturation value
/// @param p_value value value
/// @return true if is white, false if not
bool is_white(uint8_t p_hue, uint8_t p_saturation, uint8_t p_value);

/// @brief aplies hue saturation and value constraints to determine if a pixel is white
/// @param hsv_px an array with hue on pos 0, saturation on pos 1 and value on pos 2
/// @return true if is white, false if not
bool is_white(cv::Vec3b p_hsv_px);

/// @brief computes the area between three points measured in pixels
/// @param p_point1 point in the triangle
/// @param p_point2 point in the triangle
/// @param p_point3 point in the triangle
/// @return the computed area
double point_area(point p_point1, point p_point2, point p_point3);

/// @brief computes the area in a give way chunk
/// does the same thing as point_area but is nicer to use with the give way chunk struct
/// @param p_chunk a chunk with three points, between which an area is computed
/// @return the computed area
double chunk_area(give_way_chunk p_chunk);

/// @brief scales a point towards the centroid of my triangle
/// used to compute the inner triangle of my give way template
/// @param p_point the point to scale
/// @param p_centroid the centroid to get closer to 
/// @param p_scale_factor how close to get to the centroid
/// @return the new position of the current point
point scale_point_towards_centroid(point p_point, point p_centroid, double p_scale_factor);

/// @brief computes the three points of the inside white triangle in a give way sign based on the outer red trinagle points
/// @param p_chunk the give way chunk describing the outside trinangle
/// @param p_scale how much smaller should the white triangle inside be
/// @return a give way chunk struct containing the coordonates of the inner white triangle
give_way_chunk get_inner_triangle_chunk(give_way_chunk p_chunk, double p_scale);

/// @brief gets what side a point is on in relation to a vertex
/// used to determine if a point is inside a triangle
/// @param p_point the point to check on
/// @param p_vertex_p1 first point in vertex
/// @param p_vertex_p2 second point in vertex
/// @return negative number if point on one side, positive if on the other side of vertex
float get_side(point p_point, point p_vertex_p1, point p_vertex_p2);

/// @brief checks if a point is inside a trinagle
/// @param p_point the point to check on 
/// @param p_point_tr_1 point describing triangle
/// @param p_point_tr_2 point describing triangle
/// @param p_point_tr_3 point describing triangle
/// @return true if the point is inside the triangle, false otherwise
bool point_in_triangle(point p_point, point p_point_tr_1, point p_point_tr_2, point p_point_tr_3);

/// @brief checkis if the angle between any two points in a give way chunk is smaller than a threshold
/// used to filter out chunks before template matching for faster processing
/// @param p_chunk give way chunk with 3 points to check for angles
/// @return true if has at least an agle too small, false otherwise
bool has_small_angle(give_way_chunk p_chunk);

/// @brief checks if the color difference between two pixels is too big
/// used to keep pixels as part of the same chunk, even though they don't fit the is_red function
/// this way pixels that may appear orange but are very close in color to a red pixel nearby are kept in the current connex component
/// and the chunking function becomes more robust to glare
/// @param p_red_1 red component in first pixel
/// @param p_green_1 green component in first pixel
/// @param p_blue_1 blue component in first pixel
/// @param p_red_2 red component in second pixel
/// @param p_green_2 green component in second pixel
/// @param p_blue_2 blue component in second pixel
/// @return true if the two pixels are close enough in color
bool check_color_variance(uint8_t p_red_1, uint8_t p_green_1, uint8_t p_blue_1, uint8_t p_red_2, uint8_t p_green_2, uint8_t p_blue_2);

/// @brief checks if the color difference between two pixels is too big
/// used to keep pixels as part of the same chunk, even though they don't fit the is_red function
/// this way pixels that may appear orange but are very close in color to a red pixel nearby are kept in the current connex component
/// and the chunking function becomes more robust to glare
/// @param p_hsv_px_1 vector with rgb components of first pixel
/// @param p_hsv_px_2 vector with rgb components of second pixel
/// @return true if the two pixels are close enough in color
bool check_color_variance(cv::Vec3b p_rgb_px_1, cv::Vec3b p_rgb_px_2);

/// @brief draws on the image the current give way sign detected
/// @param p_img the image to write on 
/// @param gw_chunk the coordonates of the points in the sign
void print_detection(cv::Mat &p_img, give_way_chunk gw_chunk, float p_score);

void show_pic(cv::Mat p_img);

#endif