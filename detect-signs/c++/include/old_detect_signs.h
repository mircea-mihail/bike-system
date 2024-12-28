#ifndef OLD_DETECT_SIGNS
#define OLD_DETECT_SIGNS

// this file contains my previous approach to solving the chunking and template matching problem
// in short in involves looking for red points in a matrix and when finding one placing it in a 
// linked list fronteer where i would look if its neighbors are also red and labeling all of them
// with the same number then repeat.

#include <opencv2/opencv.hpp> 
#include <deque>
#include <fstream>

#include "constants.h"
#include "utility.h"
#include "memory_pool_list.h"
#include "detect_signs.h"

// iau din coltul din stanga sus labelul formei curente si verific cu asta la is red

/// @brief does a template matching between the current chunk and a generated give way sign
/// @param p_hsv_img a hsv conversion of the current image to look into for is_red() and is_white() pixel detection
/// @param p_chunk the bounds of the current give way chunk
/// @param p_label_mat the label mat to look into for filtering bad signs before tempalte matching
/// @return returns the similitude between the current chunk and a give way sign 
/// it returns 0 if any of the filter functions indicate that the current chunk is not 
/// a give way sign (like if any of the angles between triangle vertices is too small)
float check_for_gw(cv::Mat &p_hsv_img, give_way_chunk p_chunk, std::array<std::array<int, IMAGE_WIDTH>, IMAGE_HEIGHT> &p_label_mat);
 
/// @brief expands the current red pixel found by looking at its neighbors. If any of them is red, they are added
/// in a list in order to have their neighbors checked. A label matrix is also marked at their position with the current
/// label in order to know what connex component each pixel is part of
///     while building the connex component, a give way chunk struct is completed, looking for the 3 points describing 
/// a triangle: the point that is furthest left, the one furthest right and the one closest to the bottom 
///     after finding every pixel that is inside the current connex component the function calls check_for_gw
/// in order to try to find out if the component matches a give way sign
/// @param p_img the rgb image to find give way chunk in 
/// @param p_label_mat the label matrix to complete with the current pixels
/// @param p_hsv_img a hsv conversion of the current image to look into for is_red() and is_white() pixel detection
/// @param p_label the current component label
/// @param p_x the x coordonate of the current red pixel
/// @param p_y  the y coordonate of the current red pixel
/// @return returns the similitude between the current chunk and a give way sign 
/// it returns 0 if the similitude is too small or if any of the filter functions indicate that the current chunk is not 
/// a give way sign (like if any of the angles between triangle vertices is too small)
float fill_in_shape(cv::Mat &p_img, std::array<std::array<int, IMAGE_WIDTH>, IMAGE_HEIGHT> &p_label_mat, cv::Mat &p_hsv_img, int32_t p_label, double p_x, double p_y);

/// @brief goes through every pixel in p_img matrix and looks for red pixels
/// when finding one it calls fill_in_shape in order to find a connex red component around this pixel
/// it skips red pixels that have already been visited
/// @param p_img the current image to find signs in 
/// @return the numbers of give_way chunks found in the current image
uint32_t detect_gw(cv::Mat &p_img);

#endif