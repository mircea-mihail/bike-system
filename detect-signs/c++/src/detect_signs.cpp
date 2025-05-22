#include "detect_signs.h"

// 4 times faster than iterating through each pixel
void get_bright_red_mask(cv::Mat &p_hsv_img, cv::Mat &p_red_mask)
{
    cv::Scalar lower_red_1(0, BRIGHT_MIN_RED_SATURATION, VALUE_DELIMITER - VALUE_DELIMITER_OFFSET); 
    cv::Scalar upper_red_1(HUE_POSITIVE_BRIGHT_OFFSET, MAX_SATURATION, MAX_VALUE); 

    cv::Scalar lower_red_2(MAX_HUE - HUE_NEGATIVE_BRIGHT_OFFSET, BRIGHT_MIN_RED_SATURATION, VALUE_DELIMITER - VALUE_DELIMITER_OFFSET); 
    cv::Scalar upper_red_2(MAX_HUE, MAX_SATURATION, MAX_VALUE); 

    // Create two masks for the two red hue ranges
    cv::Mat mask;

    // Apply inRange to find red pixels in both ranges
    cv::inRange(p_hsv_img, lower_red_1, upper_red_1, p_red_mask); // Mask for the first red range
    cv::inRange(p_hsv_img, lower_red_2, upper_red_2, mask);
    p_red_mask |= mask;
}

void get_dark_red_mask(cv::Mat &p_hsv_img, cv::Mat &p_red_mask)
{
    // Define the lower and upper bounds for red hues in HSV
    cv::Scalar lower_red_1(0, DARK_MIN_RED_SATURATION, DARK_MIN_RED_VALUE);  
    cv::Scalar upper_red_1(HUE_POSITIVE_DARK_OFFSET, MAX_SATURATION, VALUE_DELIMITER + VALUE_DELIMITER_OFFSET); 

    cv::Scalar lower_red_2(MAX_HUE - HUE_NEGATIVE_DARK_OFFSET, DARK_MIN_RED_SATURATION, DARK_MIN_RED_VALUE);  
    cv::Scalar upper_red_2(MAX_HUE, MAX_SATURATION, VALUE_DELIMITER + VALUE_DELIMITER_OFFSET); 

    // Create two masks for the two red hue ranges
    cv::Mat mask;

    // Apply inRange to find red pixels in both ranges
    cv::inRange(p_hsv_img, lower_red_1, upper_red_1, p_red_mask); 
    cv::inRange(p_hsv_img, lower_red_2, upper_red_2, mask);
    p_red_mask |= mask;
}

void get_white_mask(cv::Mat &p_hsv_img, cv::Mat &p_white_mask)
{
    cv::Scalar lower_red_1(0, 0, MIN_WHITE_VALUE);  
    cv::Scalar upper_red_1(MAX_HUE, MAX_WHITE_SATURATION, MAX_VALUE); 

    cv::inRange(p_hsv_img, lower_red_1, upper_red_1, p_white_mask); 
}

void get_black_mask(cv::Mat &p_hsv_img, cv::Mat &p_black_mask)
{
    cv::Scalar lower_red_1(0, 0, 0);  
    cv::Scalar upper_red_1(MAX_HUE, MAX_BLACK_SATURATION, MAX_BLACK_VALUE); 

    cv::inRange(p_hsv_img, lower_red_1, upper_red_1, p_black_mask); 
}

void detect_gw_thread(cv::Mat *p_img, cv::Mat *p_red_mask, cv::Mat *p_white_mask, cv::Mat *p_black_mask,
    std::atomic<int32_t> *p_detection_number, std::vector<cv::Mat> *p_templates)
{
    cv::Mat labels;
    cv::Mat stats;
    cv::Mat centroids;
    cv::connectedComponentsWithStats(*p_red_mask, labels, stats, centroids);

    for(int i=1; i < stats.rows; i++)
    {
        float gw_detection_res = find_gw_in_chunk(*p_img, *p_white_mask, labels, stats, i);
        find_stop_in_chunk(*p_img, *p_white_mask, labels, stats, i, *p_templates);
        if(gw_detection_res > 0)
        {
            (*p_detection_number) ++;
        }
    }
}

float detect_gw_cv(cv::Mat &p_img, std::vector<cv::Mat> &p_templates)
{

    cv::Mat hsv_image;
    cv::Mat dark_red_mask, bright_red_mask;
    cv::Mat white_mask, black_mask;

    std::atomic<int32_t> detection_number = 0;
    float best_score = 0;
 
    cv::cvtColor(p_img, hsv_image, cv::COLOR_BGR2HSV);
    get_dark_red_mask(hsv_image, dark_red_mask);
    get_bright_red_mask(hsv_image, bright_red_mask);
    get_white_mask(hsv_image, white_mask);
    get_black_mask(hsv_image, black_mask);

    // show_pic(white_mask);
    // show_pic(black_mask);

    uint8_t erode_size = 3;
    cv::Mat erode_kernel = cv::Mat::ones(erode_size, erode_size, CV_8U); 
    cv::erode(dark_red_mask, dark_red_mask, erode_kernel); 
    cv::erode(bright_red_mask, bright_red_mask, erode_kernel); 

    uint8_t dilate_size = 3;
    cv::Mat dilate_kernel = cv::Mat::ones(dilate_size, dilate_size, CV_8U); 
    cv::dilate(white_mask, white_mask, dilate_kernel);
    cv::dilate(white_mask, white_mask, dilate_kernel);

    // show_pic(p_img);
    // show_pic(dark_red_mask);
    // show_pic(bright_red_mask);
    // show_pic(white_mask);

    // on avg 15.8508 milis
    // 16.36
    // 17.45
    // detect_gw_thread(&p_img, &bright_red_mask, &white_mask, &detection_number, &mtx);
    // detect_gw_thread(&p_img, &dark_red_mask, &white_mask, &detection_number, &mtx);

    // 14.12 milis
    // 14.09 milis
    // 17.94
    std::thread bright_red_gw_thread(detect_gw_thread, &p_img, &bright_red_mask, &white_mask, &black_mask, &detection_number, &p_templates);
    std::thread dark_red_gw_thread(detect_gw_thread, &p_img, &dark_red_mask, &white_mask, &black_mask, &detection_number, &p_templates);

    bright_red_gw_thread.join();
    dark_red_gw_thread.join();

    std::cout << "detection number:" << detection_number << std::endl;
    #ifdef PRINT_STATS
        std::string img_desc = "Found " + std::to_string(detection_number) + " gw signs";
        cv::Point desc_pt(10, 40);
        cv::putText(p_img, img_desc, desc_pt, 1, 3, cv::Scalar(0, 0, 0), 7);   
        cv::putText(p_img, img_desc, desc_pt, 1, 3, cv::Scalar(0, 255, 0), 3);   
    #endif

    return detection_number;
}