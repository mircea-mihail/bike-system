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
    cv::Scalar upper_red_1(MAX_HUE, MAX_SATURATION, MAX_BLACK_VALUE); 

    cv::inRange(p_hsv_img, lower_red_1, upper_red_1, p_black_mask); 
}

int detect_thread(cv::Mat *p_img, cv::Mat *p_red_mask, cv::Mat *p_white_mask, cv::Mat *p_black_mask,
    std::atomic<int32_t> *p_detection_number, std::vector<cv::Mat> *p_templates)
{
    int found_signs = NO_SIGN;
    cv::Mat labels;
    cv::Mat stats;
    cv::Mat centroids;
    cv::connectedComponentsWithStats(*p_red_mask, labels, stats, centroids);

    for(int i=1; i < stats.rows; i++)
    {
        float gw_res = find_gw_in_chunk(*p_img, *p_white_mask, labels, stats, i);
        if(gw_res > MIN_CHUNK_SCORE && ((found_signs & (1 << GIVE_WAY)) == 0)) found_signs += (1 << GIVE_WAY);

        float st_res = find_stop_in_chunk(*p_img, *p_white_mask, labels, stats, i, *p_templates);
        if(st_res > MIN_CHUNK_SCORE && ((found_signs & 1 << STOP) == 0)) found_signs += (1 << STOP);

        float nb_res = find_no_bikes_in_chunk(*p_img, *p_white_mask, *p_black_mask, labels, stats, i, *p_templates);
        if(nb_res > MIN_CHUNK_SCORE && ((found_signs & 1 << NO_BIKES) == 0)) found_signs += (1 << NO_BIKES);

        float ww_res = find_wrong_way_in_chunk(*p_img, *p_white_mask, labels, stats, i, *p_templates);
        if(ww_res > MIN_CHUNK_SCORE && ((found_signs & 1 << WRONG_WAY) == 0)) found_signs += (1 << WRONG_WAY);

        float cr_res = find_crossing_in_chunk(*p_img, *p_white_mask, *p_black_mask, labels, stats, i, *p_templates);
        if(cr_res > MIN_CHUNK_SCORE && ((found_signs & 1 << CROSSING) == 0)) found_signs += (1 << CROSSING);
    }

    return found_signs;
}

void print_detections(int p_detections)
{
    if((p_detections & (1 << GIVE_WAY)) != 0) 
        std::cout << "give way\n";

    if((p_detections & (1 << STOP)) != 0) 
        std::cout << "stop\n";

    if((p_detections & (1 << NO_BIKES)) != 0) 
        std::cout << "no bikes\n";

    if((p_detections & (1 << WRONG_WAY)) != 0) 
        std::cout << "wrong way\n";

    if((p_detections & (1 << CROSSING)) != 0) 
        std::cout << "crossing\n";

    if(p_detections != 0)
        std::cout << std::endl;
}

int merge_detections(int p_detections_1, int p_detections_2)
{
    int final_detections = NO_SIGN;

    if(((p_detections_1 & (1 << GIVE_WAY)) != 0) || (p_detections_2 & (1 << GIVE_WAY))) 
        final_detections += 1 << GIVE_WAY;

    if(((p_detections_1 & (1 << STOP)) != 0) || (p_detections_2 & (1 << STOP))) 
        final_detections += 1 << STOP;

    if(((p_detections_1 & (1 << NO_BIKES)) != 0) || (p_detections_2 & (1 << NO_BIKES))) 
        final_detections += 1 << NO_BIKES;

    if(((p_detections_1 & (1 << WRONG_WAY)) != 0) || (p_detections_2 & (1 << WRONG_WAY))) 
        final_detections += 1 << WRONG_WAY;

    if(((p_detections_1 & (1 << CROSSING)) != 0) || (p_detections_2 & (1 << CROSSING))) 
        final_detections += 1 << CROSSING;

    return final_detections;
}

int detect_signs(cv::Mat &p_img, std::vector<cv::Mat> &p_templates)
{

    cv::Mat hsv_image;
    cv::Mat dark_red_mask, bright_red_mask;
    cv::Mat white_mask, black_mask;
    int dark_detected = 0, bright_detected = 0;

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
    cv::dilate(dark_red_mask, dark_red_mask, dilate_kernel);
    cv::dilate(bright_red_mask, bright_red_mask, dilate_kernel);

    // show_pic(p_img);
    // show_pic(dark_red_mask);
    // show_pic(bright_red_mask);
    // show_pic(white_mask);
    // show_pic(black_mask);

    // on avg 15.8508 milis
    // 16.36
    // 17.45
    bright_detected = detect_thread(&p_img, &bright_red_mask, &white_mask, &black_mask, &detection_number, &p_templates);
    dark_detected = detect_thread(&p_img, &dark_red_mask, &white_mask, &black_mask, &detection_number, &p_templates);

    // 14.12 milis
    // 14.09 milis
    // 17.94
    // parralel?
    // std::thread bright_red_gw_thread(detect_gw_thread, &p_img, &bright_red_mask, &white_mask, &black_mask, &detection_number, &p_templates);
    // std::thread dark_red_gw_thread(detect_gw_thread, &p_img, &dark_red_mask, &white_mask, &black_mask, &detection_number, &p_templates);
    // bright_red_gw_thread.join();
    // dark_red_gw_thread.join();

    #ifdef PRINT_STATS
    // print all signs found
        // std::string img_desc = "Found " + std::to_string(detection_number) + " gw signs";
        // cv::Point desc_pt(10, 40);
        // cv::putText(p_img, img_desc, desc_pt, 1, 3, cv::Scalar(0, 0, 0), 7);   
        // cv::putText(p_img, img_desc, desc_pt, 1, 3, cv::Scalar(0, 255, 0), 3);   
    #endif

    return merge_detections(bright_detected, dark_detected);
}

//todo intersection over union