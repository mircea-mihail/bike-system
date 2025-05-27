#ifndef TEMPLATE_MATCH_SIGNS
#define TEMPLATE_MATCH_SIGNS

#include <opencv2/opencv.hpp> 
#include <iostream>
#include <algorithm>
#include <deque>
#include <fstream>
#include <thread>
#include <atomic>

#include "constants.h"
#include "utility.h"


float check_for_gw_cv(cv::Mat &p_white_mask, give_way_chunk p_chunk, cv::Mat &p_label_mat, int32_t p_sign_label);

float check_for_stop(cv::Mat &p_white_mask, stop_chunk st_chunk, cv::Mat &p_label_mat, int32_t p_sign_label, cv::Mat &p_stop_template);

float check_for_no_bikes(cv::Mat &p_white_mask, cv::Mat &p_black_mask, no_bikes_chunk nb_chunk, 
    cv::Mat &p_label_mat, int32_t p_sign_label, cv::Mat &p_stop_template, cv::Mat &p_img);

#endif