#ifndef FIND_SIGNS_IN_CHUNK
#define FIND_SIGNS_IN_CHUNK

#include <opencv2/opencv.hpp> 
#include <deque>
#include <fstream>
#include <thread>
#include <atomic>

#include "template_match_signs.h"
#include "constants.h"
#include "utility.h"

float find_gw_in_chunk(cv::Mat &p_img, cv::Mat &p_white_mask, cv::Mat &p_labels, cv::Mat &p_stats, int32_t p_label);

float find_stop_in_chunk(cv::Mat &p_img, cv::Mat &p_white_mask, cv::Mat &p_labels, 
    cv::Mat &p_stats, int32_t p_label, std::vector<cv::Mat> &p_templates);

#endif