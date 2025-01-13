#include "detect_signs.h"

float check_for_gw_cv(cv::Mat &p_hsv_img, give_way_chunk p_chunk, cv::Mat &p_label_mat, int32_t p_label)
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
                    cv::Vec3b px = p_hsv_img.at<cv::Vec3b>(i, j);

                    if (is_white(px[HUE], px[SATURATION], px[VALUE]) && (current_label == 0))
                    {
                        thin_white_score += 1;
                    }
                }
                else
                {
                    if (current_label != 0)
                    {
                        thin_red_score += 1;
                    }
                }

                // and check for thick border
                if (point_in_triangle(point(j, i), thick_inner_triangle.top_left, thick_inner_triangle.top_right, thick_inner_triangle.bottom))
                {
                    cv::Vec3b px = p_hsv_img.at<cv::Vec3b>(i, j);

                    if (is_white(px[HUE], px[SATURATION], px[VALUE]) && (current_label == 0))
                    { 
                        thick_white_score += 1;
                    }
                }
                else
                {
                    if (current_label != 0)
                    {
                        thick_red_score += 1;
                    }
                }
            }
            // if not in triangle but red
            else 
            {
                cv::Vec3b px = p_hsv_img.at<cv::Vec3b>(i, j);

                if (current_label == p_label)
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

float find_sign(cv::Mat &p_img, cv::Mat &p_hsv_img, cv::Mat &p_labels, cv::Mat &p_stats, int32_t p_label)
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

    point bottom_pt;
    for (int j = x; j < x + w; j++)
    {
        if (p_labels.at<int32_t>(y + h-1, j) == p_label)
        {
            bottom_pt.x = j;
            bottom_pt.y = y + h-1;
            break;
        }
    }

    give_way_chunk gw_chunk = give_way_chunk(left_pt, right_pt, bottom_pt);

    float chunk_score = check_for_gw_cv(p_hsv_img, gw_chunk, p_labels, p_label);
    if(chunk_score > MIN_CHUNK_SCORE)
    {
        #ifdef PRINT_STATS
            print_detection(p_img, gw_chunk, chunk_score);
        #endif

        return chunk_score;
    }

    return 0;
}

// 4 times faster than iterating through each pixel
void get_red_pixels(cv::Mat &p_hsv_img, cv::Mat &p_red_pixels)
{
    // Define the lower and upper bounds for red hues in HSV
    cv::Scalar lower_red_1(0, DARK_MIN_RED_SATURATION, DARK_MIN_RED_VALUE);  
    cv::Scalar upper_red_1(HUE_POSITIVE_DARK_OFFSET, MAX_SATURATION, VALUE_DELIMITER); 

    cv::Scalar lower_red_2(0, BRIGHT_MIN_RED_SATURATION, VALUE_DELIMITER); 
    cv::Scalar upper_red_2(HUE_POSITIVE_BRIGHT_OFFSET, MAX_SATURATION, MAX_VALUE); 

    cv::Scalar lower_red_3(MAX_HUE - HUE_NEGATIVE_DARK_OFFSET, DARK_MIN_RED_SATURATION, DARK_MIN_RED_VALUE);  
    cv::Scalar upper_red_3(MAX_HUE, MAX_SATURATION, VALUE_DELIMITER); 

    cv::Scalar lower_red_4(MAX_HUE - HUE_NEGATIVE_BRIGHT_OFFSET, BRIGHT_MIN_RED_SATURATION, VALUE_DELIMITER); 
    cv::Scalar upper_red_4(MAX_HUE, MAX_SATURATION, MAX_VALUE); 

    // Create two masks for the two red hue ranges
    cv::Mat mask;

    // Apply inRange to find red pixels in both ranges
    cv::inRange(p_hsv_img, lower_red_1, upper_red_1, p_red_pixels); // Mask for the first red range
    cv::inRange(p_hsv_img, lower_red_2, upper_red_2, mask);
    p_red_pixels |= mask;

    cv::inRange(p_hsv_img, lower_red_3, upper_red_3, mask);
    p_red_pixels |= mask;

    cv::inRange(p_hsv_img, lower_red_4, upper_red_4, mask);
    p_red_pixels |= mask;
}

float detect_gw_cv(cv::Mat &p_img)
{
    cv::Mat hsv_image;
    cv::cvtColor(p_img, hsv_image, cv::COLOR_BGR2HSV);
    cv::Mat red_pixels;
    get_red_pixels(hsv_image, red_pixels);

    uint8_t erode_size = 3;
    cv::Mat kernel = cv::Mat::ones(erode_size, erode_size, CV_8U); 
    cv::erode(red_pixels, red_pixels, kernel); 

    cv::Mat labels;
    cv::Mat stats;
    cv::Mat centroids;
    cv::connectedComponentsWithStats(red_pixels, labels, stats, centroids);

    float best_score = 0;
    int32_t detection_number = 0;
    for(int i=1; i<stats.rows; i++)
    {
        float detection_res = find_sign(p_img, hsv_image, labels, stats, i);
        if(detection_res > 0)
        {
            if(best_score < detection_res)
            {
                best_score = detection_res;
            }

            detection_number ++;
        }
    }
    #ifdef PRINT_STATS
        std::string img_desc = "Found " + std::to_string(detection_number) + " gw signs";
        cv::Point desc_pt(10, 40);
        cv::putText(p_img, img_desc, desc_pt, 1, 3, cv::Scalar(0, 0, 0), 7);   
        cv::putText(p_img, img_desc, desc_pt, 1, 3, cv::Scalar(0, 255, 0), 3);   
    #endif

    return best_score;
}