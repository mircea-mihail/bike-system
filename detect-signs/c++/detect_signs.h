#ifndef DETECT_SIGNS_H
#define DETECT_SIGNS_H

#include <opencv2/opencv.hpp> 
#include <deque>
#include <fstream>

#include "constants.h"
#include "utility.h"
#include "memory_pool_list.h"

// iau din coltul din stanga sus labelul formei curente si verific cu asta la is red
float check_for_gw_cv(cv::Mat &p_hsv_img, give_way_chunk p_chunk, cv::Mat &p_label_mat, int32_t p_label);

void print_detection(cv::Mat &p_img, give_way_chunk gw_chunk);

float find_sign(cv::Mat &p_img, cv::Mat &p_hsv_img, cv::Mat &p_labels, cv::Mat &p_stats, int32_t p_pos);

// 4 times faster than iterating through each pixel
void get_red_pixels(cv::Mat &p_hsv_img, cv::Mat &p_red_pixels);

uint32_t detect_gw_cv(cv::Mat &p_img, cv::Mat &p_red_pixels);

#endif