#ifndef DETECT_SIGNS_H
#define DETECT_SIGNS_H

#include <opencv2/opencv.hpp> 
#include <deque>
#include <fstream>

#include "constants.h"
#include "utility.h"

/// @brief checks how well the current chunk matches a generated give way sign between the chunk points
/// @param p_hsv_img the hsv image, used for is_white function
/// @param p_chunk the give way chunk
/// @param p_label_mat the label matrix, used instead of calling is_red() on the current pixel
/// @param p_label the current label, used to invalidate shapes that have too much red outside the current chunk, 
/// that are connex to the sign
/// @return how well the current chunk matches a give way sign
float check_for_gw_cv(cv::Mat &p_hsv_img, give_way_chunk p_chunk, cv::Mat &p_label_mat, int32_t p_label);

/// @brief builds the gw_chunk using the data provided by the opencv connectedComponentsWithStats function 
/// and calls the check_for_gw_cv function to do a template matching on the chunk
/// @param p_img the original image, passed to visualise detections by drawing directly on it
/// @param p_hsv_img the hsv image, used for is_white() funciton in check for gw 
/// @param p_labels the label matrix
/// @param p_stats the stats built by the connectedComponentsWithStats function, used for chunk building
/// @param p_label the current label in the p_labes mat
/// @return returns the similitude between the current chunk and a give way sign
float find_gw_in_chunk(cv::Mat &p_img, cv::Mat &p_hsv_img, cv::Mat &p_labels, cv::Mat &p_stats, int32_t p_label);

/// @brief builds 2 scalar ranges used to create 2 masks in order to apply all of the hsv constraints for the bright red pixels
/// 4 times faster than iterating through each pixel and calling is_red()
/// @param p_hsv_img the original imge converted to hsv
/// @param p_red_pixels the final mask to store red pixles in (grayscale, 255 if red, 0 otherwise)
void get_bright_red_mask(cv::Mat &p_hsv_img, cv::Mat &p_red_pixels);

/// @brief builds 2 scalar ranges used to create 2 masks in order to apply all of the hsv constraints for the dark red pixels
/// @param p_hsv_img the original imge converted to hsv
/// @param p_red_pixels the final mask to store red pixles in (grayscale, 255 if red, 0 otherwise)
void get_dark_red_mask(cv::Mat &p_hsv_img, cv::Mat &p_red_pixels);

/// @brief gets a mask of the red pixels in the image, aplies erosion to it, 
/// gets all the connex components in the mask using an opencv function and if they are big enough 
/// tries to find a give way sign in the current connex components
/// @param p_img the rgb image to try to find give way sign in 
/// @return the best score found in the img
float detect_gw_cv(cv::Mat &p_img);

#endif