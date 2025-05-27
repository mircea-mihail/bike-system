#ifndef UTILITY_H
#define UTILITY_H

#include <cstdint>

#include "constants.h"

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

bool has_small_angle(stop_chunk p_chunk);

bool has_small_angle(no_bikes_chunk p_chunk);

/// @brief draws on the image the current give way sign detected
/// @param p_img the image to write on 
/// @param gw_chunk the coordonates of the points in the sign
void print_give_way(cv::Mat &p_img, give_way_chunk gw_chunk, float p_score);

/// @brief draws on the image the current stop sign detected
/// @param p_img the image to write on 
/// @param gw_chunk the coordonates of the points in the sign
void print_stop(cv::Mat &p_img, stop_chunk st_chunk, float p_score);

void show_pic(cv::Mat p_img);

cv::Point2f get_line_intersection( point A, point B, point C, point D);

float get_angle(point A, point B, point C);

void print_bounding_box(cv::Mat &p_img, int32_t p_x, int32_t p_y, int32_t p_w, int32_t p_h);

void print_no_bikes(cv::Mat &p_img, no_bikes_chunk nb_chunk, float p_score);

#endif