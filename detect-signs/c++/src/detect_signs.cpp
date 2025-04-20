#include "detect_signs.h"

float check_for_gw_cv(cv::Mat &p_white_mask, give_way_chunk p_chunk, cv::Mat &p_label_mat, int32_t p_sign_label)
{
    // filter out triangles with angle smaller than minimum
    if(has_small_angle(p_chunk))
    {
        return 0;
    }

    int32_t px_outside_gw = 0;
    int32_t red_outside_gw = 0;

    int32_t thin_red_score = 0;
    int32_t thin_white_score = 0;
    int32_t thick_red_score = 0;
    int32_t thick_white_score = 0;

    int32_t pixels_checked = 0;

    point view_win_top_left = point(std::min(p_chunk.top_left.x, p_chunk.bottom.x), std::min(p_chunk.top_left.y, p_chunk.top_right.y));
    point view_win_btm_right = point(std::max(p_chunk.top_right.x, p_chunk.bottom.x), p_chunk.bottom.y);

    give_way_chunk thin_inner_triangle = get_inner_triangle_chunk(p_chunk, THIN_GW_BORDER_PER_CENT);
    give_way_chunk thick_inner_triangle = get_inner_triangle_chunk(p_chunk, THICK_GW_BORDER_PER_CENT);

    // Draw the smaller triangle
    for(int i = int(view_win_top_left.y); i <= int(view_win_btm_right.y); i++)
    {
        bool found_triangle_in_line = false;

        for(int j = int(view_win_top_left.x); j <= int(view_win_btm_right.x); j++)
        {
            int32_t current_label = p_label_mat.at<int32_t>(i, j);

            if (point_in_triangle(point(j, i), p_chunk.top_left, p_chunk.top_right, p_chunk.bottom))
            {
                found_triangle_in_line = true;
                pixels_checked += 1;

                // check for thin border
                if (point_in_triangle(point(j, i), thin_inner_triangle.top_left, thin_inner_triangle.top_right, thin_inner_triangle.bottom))
                {
                    if (p_white_mask.at<int8_t>(i, j) != 0 && (current_label == 0))
                    {
                        thin_white_score += 1;
                    }
                }
                else
                {
                    if (current_label == p_sign_label)
                    {
                        thin_red_score += 1;
                    }
                }

                // and check for thick border
                if (point_in_triangle(point(j, i), thick_inner_triangle.top_left, thick_inner_triangle.top_right, thick_inner_triangle.bottom))
                {
                    if (p_white_mask.at<int8_t>(i, j) != 0 && (current_label == 0))
                    { 
                        thick_white_score += 1;
                    }
                }
                else
                {
                    if (current_label == p_sign_label)
                    {
                        thick_red_score += 1;
                    }
                }
            }
            // if not in triangle but red
            else 
            {
                if (current_label == p_sign_label)
                {
                    red_outside_gw += 1;
                }

                px_outside_gw += 1;
            }
        }
    }

    // if there are a lot of red pixels outside of triangle, likely not a give way
    if(px_outside_gw != 0)
    {
        if ( red_outside_gw / (double)px_outside_gw > RED_OUTSIDE_GW_THRESHOLD)
        {
            // too much red outside
            return 0;
        }
 
    }
  
    // get final score for thin
    double thin_white_area = chunk_area(thin_inner_triangle);
    double thin_red_area = (chunk_area(p_chunk) - thin_white_area);
    double thin_white_checked = thin_white_area / (thin_white_area + thin_red_area) * pixels_checked;
    double thin_red_checked = pixels_checked - thin_white_checked;
    double final_thin_score = (std::min(thin_white_score / thin_white_checked, 1.0) + std::min(thin_red_score / thin_red_checked, 1.0)) / 2;

    // get final score for thick 
    double thick_white_area = chunk_area(thick_inner_triangle);
    double thick_red_area = (chunk_area(p_chunk) - thick_white_area) ;
    double thick_white_checked = thick_white_area / (thick_white_area + thick_red_area) * pixels_checked;
    double thick_red_checked = pixels_checked - thick_white_checked;
    double final_thick_score = (std::min(thick_white_score / thick_white_checked, 1.0) + std::min(thick_red_score / thick_red_checked, 1.0)) / 2;

    // std::cout << "best sign score: " << std::max(final_thin_score, final_thick_score) << std::endl;
    // std::cout << "\tthick max whtie:\t\t" << thick_white_score/thick_white_checked << " red:\t"<< thick_red_score/thick_red_checked << std::endl;
    // std::cout << "\tthin max whtie: \t\t" << thin_white_score/thin_white_checked << " red:\t"<< thin_red_score/thin_red_checked << std::endl;

    return std::max(final_thin_score, final_thick_score);
}

void print_bounding_box(cv::Mat &p_img, int32_t p_x, int32_t p_y, int32_t p_w, int32_t p_h)
{
    // print a bounding box above all checked shapes
    cv::Scalar color(255,0,0);
    cv::Rect rect(p_x, p_y, p_w, p_h);
    uint8_t thickness = 2;
    cv::rectangle(p_img, rect, color, thickness);
}

float find_gw_in_chunk(cv::Mat &p_img, cv::Mat &p_white_mask, cv::Mat &p_labels, cv::Mat &p_stats, int32_t p_label)
{
    int32_t x = p_stats.at<int32_t>(cv::Point(0, p_label));
    int32_t y = p_stats.at<int32_t>(cv::Point(1, p_label));
    int32_t w = p_stats.at<int32_t>(cv::Point(2, p_label));
    int32_t h = p_stats.at<int32_t>(cv::Point(3, p_label));

    if (h * w < MIN_BOUNDING_BOX_AREA)
    {
        return 0;
    }

    #ifdef PRINT_STATS
        print_bounding_box(p_img, x, y, w, h);
    #endif

    point left_pt;
    for (int i = y; i < y + h; i++)
    {
        if (p_labels.at<int32_t>(i, x) == p_label)
        {
            left_pt.x = x;
            left_pt.y = i;
            break;
        }
    }

    point right_pt;
    for (int i = y; i < y + h; i++)
    {
        if (p_labels.at<int32_t>(i, x + w-1) == p_label)
        {
            right_pt.x = x + w-1;
            right_pt.y = i;
            break;
        }
    }

    point leftest_bottom_pt(-1, -1);
    point rightest_bottom_pt;
    for (int j = x; j < x + w; j++)
    {
        if (p_labels.at<int32_t>(y + h-1, j) == p_label)
        {
            rightest_bottom_pt.x = j;
            rightest_bottom_pt.y = y + h-1;
            if(leftest_bottom_pt.x == -1)
            {
                leftest_bottom_pt.x = j;
                leftest_bottom_pt.y = y + h-1;
            }
            break;
        }
    }
    point bottom_pt(int((rightest_bottom_pt.x + leftest_bottom_pt.x) / 2), rightest_bottom_pt.y);

    give_way_chunk gw_chunk = give_way_chunk(left_pt, right_pt, bottom_pt);

    float chunk_score = check_for_gw_cv(p_white_mask, gw_chunk, p_labels, p_label);
    if(chunk_score > MIN_CHUNK_SCORE)
    {
        #ifdef PRINT_STATS
            print_detection(p_img, gw_chunk, chunk_score);
        #endif

        return chunk_score;
    }

    return 0;
}

float find_stop_in_chunk(cv::Mat &p_img, cv::Mat &p_white_mask, cv::Mat &p_labels, cv::Mat &p_stats, int32_t p_label)
{
    int32_t x = p_stats.at<int32_t>(cv::Point(0, p_label));
    int32_t y = p_stats.at<int32_t>(cv::Point(1, p_label));
    int32_t w = p_stats.at<int32_t>(cv::Point(2, p_label));
    int32_t h = p_stats.at<int32_t>(cv::Point(3, p_label));

    if (h * w < MIN_BOUNDING_BOX_AREA)
    {
        return 0;
    }

    #ifdef PRINT_STATS
        print_bounding_box(p_img, x, y, w, h);
    #endif

    point top_left(x, y);
    point top_right(x, y);
    float_t top_left_val = IMAGE_HEIGHT * IMAGE_WIDTH * 2;
    float_t top_right_val = IMAGE_HEIGHT * IMAGE_WIDTH * -2;

    point bottom_left(x, y);
    point bottom_right(x, y);
    float_t bottom_left_val = IMAGE_HEIGHT * IMAGE_WIDTH * -2;
    float_t bottom_right_val = IMAGE_HEIGHT * IMAGE_WIDTH * -2;

    point left_bottom(x, y);
    point left_top(x, y);
    float_t left_bottom_val = IMAGE_HEIGHT * IMAGE_WIDTH * 2;
    float_t left_top_val = IMAGE_HEIGHT * IMAGE_WIDTH * 2;

    point right_bottom(x, y);
    point right_top(x, y);
    float_t right_bottom_val = IMAGE_HEIGHT * IMAGE_WIDTH * -2;
    float_t right_top_val = IMAGE_HEIGHT * IMAGE_WIDTH * -2;
 
    float_t bias = 0.4;
    for (int i = y; i < y + h; i++)
    {
        for (int j = x; j < x + w; j++)
        {
            if (p_labels.at<int32_t>(i, j) == p_label)
            {
                if(i + bias * j < top_left_val)
                {
                    top_left_val = i + bias * j;
                    top_left.x = j;
                    top_left.y = i;
                }
                if( bias * j - i > top_right_val)
                {
                    top_right_val = bias * j - i;
                    top_right.x = j;
                    top_right.y = i;
                }

                if(i - bias * j > bottom_left_val)
                {
                    bottom_left_val = i - bias * j;
                    bottom_left.x = j;
                    bottom_left.y = i;
                }
                if( bias * j + i > bottom_right_val)
                {
                    bottom_right_val = bias * j + i;
                    bottom_right.x = j;
                    bottom_right.y = i;
                }

                if(bias * i + j < left_top_val)
                {
                    left_top_val = j + bias * i;
                    left_top.x = j;
                    left_top.y = i;
                }
                if(j - bias * i < left_bottom_val)
                {
                    left_bottom_val = j - bias * i;
                    left_bottom.y = i;
                    left_bottom.x = j;
                }

                if( j + bias * i > right_bottom_val)
                {
                    right_bottom_val = j + bias * i;
                    right_bottom.x = j;
                    right_bottom.y = i;
                }
                if( j - bias * i > right_top_val)
                {
                    right_top_val = j - bias * i;
                    right_top.x = j;
                    right_top.y = i;
                }
            }
        }
    }

    cv::Scalar color = cv::Scalar(0, 255, 0);
    int thickness = 6;
    cv::line(p_img, cv::Point2d(top_left.x, top_left.y), cv::Point2d(top_right.x, top_right.y), color, thickness);
    cv::line(p_img, cv::Point2d(bottom_left.x, bottom_left.y), cv::Point2d(bottom_right.x, bottom_right.y), color, thickness);
    cv::line(p_img, cv::Point2d(left_bottom.x, left_bottom.y), cv::Point2d(left_top.x, left_top.y), color, thickness);
    cv::line(p_img, cv::Point2d(right_bottom.x, right_bottom.y), cv::Point2d(right_top.x, right_top.y), color, thickness);

    point left_pt;
    for (int i = y; i < y + h; i++)
    {
        if (p_labels.at<int32_t>(i, x) == p_label)
        {
            left_pt.x = x;
            left_pt.y = i;
            break;
        }
    }

    point right_pt;
    for (int i = y; i < y + h; i++)
    {
        if (p_labels.at<int32_t>(i, x + w-1) == p_label)
        {
            right_pt.x = x + w-1;
            right_pt.y = i;
            break;
        }
    }

    point leftest_bottom_pt(-1, -1);
    point rightest_bottom_pt;
    for (int j = x; j < x + w; j++)
    {
        if (p_labels.at<int32_t>(y + h-1, j) == p_label)
        {
            rightest_bottom_pt.x = j;
            rightest_bottom_pt.y = y + h-1;
            if(leftest_bottom_pt.x == -1)
            {
                leftest_bottom_pt.x = j;
                leftest_bottom_pt.y = y + h-1;
            }
            break;
        }
    }
    point bottom_pt(int((rightest_bottom_pt.x + leftest_bottom_pt.x) / 2), rightest_bottom_pt.y);

    give_way_chunk gw_chunk = give_way_chunk(left_pt, right_pt, bottom_pt);

    // float chunk_score = check_for_gw_cv(p_white_mask, gw_chunk, p_labels, p_label);
    // if(chunk_score > MIN_CHUNK_SCORE)
    // {
    //     #ifdef PRINT_STATS
    //         print_detection(p_img, gw_chunk, chunk_score);
    //     #endif

    //     return chunk_score;
    // }

    return 0;
}

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
    cv::inRange(p_hsv_img, lower_red_1, upper_red_1, p_red_mask); // Mask for the first red range
    cv::inRange(p_hsv_img, lower_red_2, upper_red_2, mask);
    p_red_mask |= mask;
}

void get_white_mask(cv::Mat &p_hsv_img, cv::Mat &p_white_mask)
{
    // Define the lower and upper bounds for red hues in HSV
    cv::Scalar lower_red_1(0, 0, MIN_WHITE_VALUE);  
    cv::Scalar upper_red_1(MAX_HUE, MAX_WHITE_SATURATION, MAX_VALUE); 

    // Apply inRange to find red pixels in both ranges
    cv::inRange(p_hsv_img, lower_red_1, upper_red_1, p_white_mask); // Mask for the first red range
}

void get_masks(cv::Mat &p_img, cv::Mat &p_red_mask, cv::Mat &p_white_mask)
{

}

void detect_gw_thread(cv::Mat *p_img, cv::Mat *p_red_mask,
    cv::Mat *p_white_mask, std::atomic<int32_t> *p_detection_number)
{
    cv::Mat labels;
    cv::Mat stats;
    cv::Mat centroids;
    cv::connectedComponentsWithStats(*p_red_mask, labels, stats, centroids);

    for(int i=1; i < stats.rows; i++)
    {
        float gw_detection_res = find_gw_in_chunk(*p_img, *p_white_mask, labels, stats, i);
        find_stop_in_chunk(*p_img, *p_white_mask, labels, stats, i);
        if(gw_detection_res > 0)
        {
            (*p_detection_number) ++;
        }
    }
}

float detect_gw_cv(cv::Mat &p_img, std::vector<cv::Mat> &p_templates)
{

    cv::Mat hsv_image;
    cv::Mat dark_red_mask;
    cv::Mat bright_red_mask;
    cv::Mat white_mask;

    std::atomic<int32_t> detection_number = 0;
    float best_score = 0;
 
    cv::cvtColor(p_img, hsv_image, cv::COLOR_BGR2HSV);
    get_dark_red_mask(hsv_image, dark_red_mask);
    get_bright_red_mask(hsv_image, bright_red_mask);
    get_white_mask(hsv_image, white_mask);

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
    std::thread bright_red_gw_thread(detect_gw_thread, &p_img, &bright_red_mask, &white_mask, &detection_number);
    std::thread dark_red_gw_thread(detect_gw_thread, &p_img, &dark_red_mask, &white_mask, &detection_number);

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