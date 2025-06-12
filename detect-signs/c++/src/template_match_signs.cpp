#include "template_match_signs.h"

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
                    if (p_white_mask.at<uchar>(i, j) > 0 && (current_label == 0))
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
                    if (p_white_mask.at<uchar>(i, j) > 0 && (current_label == 0))
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
    if(px_outside_gw > 0)
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

float check_for_stop(cv::Mat &p_white_mask, stop_chunk st_chunk, cv::Mat &p_label_mat, 
    int32_t p_sign_label, cv::Mat &p_stop_template)
{
    if(has_small_angle(st_chunk))
    {
        return 0;
    }

    cv::Point2f top_left_corner = get_line_intersection(st_chunk.top_right, st_chunk.top_left, st_chunk.left_bottom, st_chunk.left_top);   
    cv::Point2f top_right_corner = get_line_intersection(st_chunk.top_right, st_chunk.top_left, st_chunk.right_bottom, st_chunk.right_top);   
    cv::Point2f bottom_left_corner = get_line_intersection(st_chunk.bottom_right, st_chunk.bottom_left, st_chunk.left_bottom, st_chunk.left_top);   
    cv::Point2f bottom_right_corner = get_line_intersection(st_chunk.bottom_right, st_chunk.bottom_left, st_chunk.right_bottom, st_chunk.right_top);   

    std::vector<cv::Point2f> src_points = {
        top_left_corner,
        top_right_corner,
        bottom_right_corner,
        bottom_left_corner
	};

	int32_t width = STOP_TEMPLATE_WIDTH;
	int32_t height = STOP_TEMPLATE_HEIGHT;
	
	std::vector<cv::Point2f> dst_points = {
		cv::Point2f(0, 0),           
		cv::Point2f(width - 1, 0),    
		cv::Point2f(width - 1, height - 1),
		cv::Point2f(0, height - 1)    
	};
	cv::Mat H = cv::getPerspectiveTransform(src_points, dst_points);

	cv::Mat warped_red_labels;
    cv::Mat warped_white_mask;
    cv::Mat float_labels;
    p_label_mat.convertTo(float_labels, CV_32F);
	cv::warpPerspective(float_labels, warped_red_labels, H, cv::Size(width, height), cv::INTER_NEAREST );
    
	cv::warpPerspective(p_white_mask, warped_white_mask, H, cv::Size(width, height), cv::INTER_NEAREST );
    warped_red_labels.convertTo(warped_red_labels, CV_32S);
    
    float_t red_score = 0, red_total = 0, white_score = 0, white_total = 0;
    float_t red_outside = 0, outside_total = 0;
    double px_max_val = 255;

    for(int i = 0; i < STOP_TEMPLATE_HEIGHT; i++)
    {
        for(int j = 0; j < STOP_TEMPLATE_WIDTH; j++)
        {
            // if not green
            if (! ((p_stop_template.at<cv::Vec3b>(i, j)[GREEN]) > COLOR_THRESHOLD
                && (p_stop_template.at<cv::Vec3b>(i, j)[RED]) < COLOR_THRESHOLD
                && (p_stop_template.at<cv::Vec3b>(i, j)[BLUE]) < COLOR_THRESHOLD)
        )
            {
                uchar red_px = p_stop_template.at<cv::Vec3b>(i, j)[RED];
                uchar green_px = p_stop_template.at<cv::Vec3b>(i, j)[GREEN];
                uchar blue_px = p_stop_template.at<cv::Vec3b>(i, j)[BLUE];

                // score according to template
                red_total += cv::max((red_px - (green_px + blue_px) / 2) / px_max_val, 0.0);
                if(warped_red_labels.at<int32_t>(i, j) == p_sign_label)
                {
                    red_score += cv::max((red_px - (green_px + blue_px) / 2) / px_max_val, 0.0);
                }

                if (red_px > COLOR_THRESHOLD && green_px > COLOR_THRESHOLD && blue_px > COLOR_THRESHOLD)
                {
                    white_total += cv::min({red_px, green_px, blue_px}) / px_max_val;
                    if (warped_red_labels.at<int32_t>(i, j) != p_sign_label && warped_white_mask.at<uchar>(i, j) > 0)
                    {
                        white_score += cv::min({red_px, green_px, blue_px}) / px_max_val;
                    }
                }
            }
            else
            {
                outside_total ++;
                if(warped_red_labels.at<int32_t>(i, j) == p_sign_label)
                {
                    red_outside ++;
                }
            }
        }
    }

    if(red_total == 0 || white_total == 0 || red_outside/outside_total > RED_OUTSIDE_STOP_THRESHOLD)
    {
        return 0;
    }

    std::cout << "white score: " << white_score << "\nwhite total: " << white_total << std::endl;
    std::cout << "red score: " << red_score << "\nred total: " << red_total << std::endl;
    std::cout << "outside ratio: " << red_outside/outside_total << std::endl;
    std::cout << "final score:" << 0.5 * (red_score/red_total) + 0.5 * (white_score/white_total) << std::endl;
    std::cout << std::endl;

    return 0.5 * (red_score/red_total) + 0.5 * (white_score/white_total);
}

double get_rotation_angle_to_horizontal(cv::Mat p_warped_black_mask)
{
    // isolate the circle where the bike is supposed to be using a mask
    cv::Mat mask = cv::Mat::zeros(p_warped_black_mask.size(), CV_8UC1);

    int radius = 36;
    cv::Point center(mask.cols / 2, mask.rows / 2);
    cv::circle(mask, center, radius, cv::Scalar(255), cv::FILLED);
    // nice prints
    // show_pic(mask);

    cv::Mat clean_black_mask;
    cv::bitwise_and(p_warped_black_mask, mask, clean_black_mask);

    cv::Mat labels, stats, centroids;
    int num_labels = cv::connectedComponentsWithStats(clean_black_mask, labels, stats, centroids);

    // get the biggest area chunk as it has the cyclist symbol
    int max_area = 0;
    int max_label = 1;
    for (int i = 1; i < num_labels; ++i) {
        int area = stats.at<int>(i, cv::CC_STAT_AREA);
        if (area > max_area) {
            max_area = area;
            max_label = i;
        }
    }

    int32_t x = stats.at<int32_t>(cv::Point(0, max_label));
    int32_t y = stats.at<int32_t>(cv::Point(1, max_label));
    int32_t w = stats.at<int32_t>(cv::Point(2, max_label));
    int32_t h = stats.at<int32_t>(cv::Point(3, max_label));

    // get wheel coordonates
    cv::Point2i left_wheel_pt, right_wheel_pt;
    for(int i = y; i < y + h; i++)
    {
        if(clean_black_mask.at<uint8_t>(i, x) != 0)
        {
            left_wheel_pt = cv::Point2i(x, i);
        }

        if(clean_black_mask.at<uint8_t>(i, x + w - 1) != 0)
        {
            right_wheel_pt = cv::Point2i(x + w - 1, i);
        }
        
    }

    // nice prints
    // show_pic(clean_black_mask);
    cv::circle(clean_black_mask, left_wheel_pt, 1, cv::Scalar(128), cv::FILLED);
    cv::circle(clean_black_mask, right_wheel_pt, 1, cv::Scalar(128), cv::FILLED);
    // show_pic(clean_black_mask);

    // get slope between the leftest and rightest points:
    int dy = right_wheel_pt.y - left_wheel_pt.y;
    int dx = right_wheel_pt.x - left_wheel_pt.x; 

    double angle_rad = std::atan2(dy, dx);
    double angle_deg = angle_rad * 180.0 / M_PI;

    // convert from slope angle to degrees to turn the image by
    double slope_min=-30, slope_max=30, theta_min=-45, theta_max=45;
    return (angle_deg - slope_min) / (slope_max - slope_min) * (theta_max - theta_min) + theta_min;
}

// todo delete p_img
float check_for_no_bikes(cv::Mat &p_white_mask, cv::Mat &p_black_mask, no_bikes_chunk nb_chunk, 
    cv::Mat &p_label_mat, int32_t p_sign_label, cv::Mat &p_nb_template, cv::Mat &p_img)
{
    if(has_small_angle(nb_chunk))
    {
        return 0;
    }
    // show_pic(p_img);
    // show_pic(p_white_mask);

    cv::Point2f top_left_corner = cv::Point2f(nb_chunk.left.x, nb_chunk.top.y);
    cv::Point2f top_right_corner = cv::Point2f(nb_chunk.right.x, nb_chunk.top.y);
    cv::Point2f bottom_left_corner = cv::Point2f(nb_chunk.left.x, nb_chunk.bottom.y);
    cv::Point2f bottom_right_corner = cv::Point2f(nb_chunk.right.x, nb_chunk.bottom.y);

    std::vector<cv::Point2f> src_points = {
        top_left_corner,
        top_right_corner,
        bottom_right_corner,
        bottom_left_corner
	};

	int32_t width = p_nb_template.cols;
	int32_t height = p_nb_template.rows;
	
	std::vector<cv::Point2f> dst_points = {
		cv::Point2f(0, 0),           
		cv::Point2f(width - 1, 0),    
		cv::Point2f(width - 1, height - 1),
		cv::Point2f(0, height - 1)    
	};
	cv::Mat H = cv::getPerspectiveTransform(src_points, dst_points);

	cv::Mat warped_red_labels;
    cv::Mat warped_white_mask;
    cv::Mat warped_black_mask;

    cv::Mat float_labels;
    cv::Mat ref_img;
    p_label_mat.convertTo(float_labels, CV_32F);

	cv::warpPerspective(float_labels, warped_red_labels, H, cv::Size(width, height), cv::INTER_NEAREST );
	cv::warpPerspective(p_black_mask, warped_black_mask, H, cv::Size(width, height), cv::INTER_NEAREST );
	cv::warpPerspective(p_white_mask, warped_white_mask, H, cv::Size(width, height), cv::INTER_NEAREST );
	cv::warpPerspective(p_img, ref_img, H, cv::Size(width, height), cv::INTER_NEAREST );

    //todo remove vibe check
    cv::Mat vibe_check = warped_white_mask.clone();

    // nice prints 
    // show_pic(ref_img);
    // show_pic(warped_black_mask);

    // show_pic(p_nb_template);
    // show_pic(warped_red_labels);
    // show_pic(warped_white_mask);


    float deskew_angle = get_rotation_angle_to_horizontal(warped_black_mask);

    // rotate mask in order to have it horizontal
    cv::Point2f center(warped_black_mask.cols / 2.0F, warped_black_mask.rows / 2.0F);
    double scale = 1.0;
    cv::Mat rotationMatrix = cv::getRotationMatrix2D(center, deskew_angle, scale);
    // Compute the bounding box of the rotated image to prevent clipping

    cv::warpAffine(warped_black_mask, warped_black_mask, rotationMatrix, warped_black_mask.size());


    warped_red_labels.convertTo(warped_red_labels, CV_32S);
    
    float_t red_score = 0, red_total = 0;
    float_t white_score = 0, white_total = 0;
    float_t thick_black_score = 0, thin_black_score = 0, black_total = 0;
    float_t red_outside = 0, outside_total = 0;
    double px_max_val = 255.0;

    // also check for a thicker black bike and choose the better score
    cv::Mat thick_black_mask;
    warped_black_mask.copyTo(thick_black_mask);

    uint8_t dilate_size = 3;
    cv::Mat dilate_kernel = cv::Mat::ones(dilate_size, dilate_size, CV_8U); 
    cv::dilate(thick_black_mask, thick_black_mask, dilate_kernel);

    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            vibe_check.at<uchar>(i, j) = 0;
            // if not green
            if (! ((p_nb_template.at<cv::Vec3b>(i, j)[GREEN]) > COLOR_THRESHOLD
                && (p_nb_template.at<cv::Vec3b>(i, j)[RED]) < COLOR_THRESHOLD
                && (p_nb_template.at<cv::Vec3b>(i, j)[BLUE]) < COLOR_THRESHOLD))
            {
                int32_t red_px = p_nb_template.at<cv::Vec3b>(i, j)[RED];
                int32_t green_px = p_nb_template.at<cv::Vec3b>(i, j)[GREEN];
                int32_t blue_px = p_nb_template.at<cv::Vec3b>(i, j)[BLUE];

                // score according to template
                red_total += std::max((red_px - (green_px + blue_px) / 2) / px_max_val, 0.0);
                if(warped_red_labels.at<int32_t>(i, j) == p_sign_label)
                {
                    red_score += std::max((red_px - (green_px + blue_px) / 2) / px_max_val, 0.0);
                    vibe_check.at<uchar>(i, j) = std::min(std::max((red_px - (green_px + blue_px) / 2) / px_max_val, 0.0) * px_max_val, px_max_val);
                }

                if (red_px > COLOR_THRESHOLD && green_px > COLOR_THRESHOLD && blue_px > COLOR_THRESHOLD)
                {
                    white_total += cv::min({red_px, green_px, blue_px}) / px_max_val;

                    if (warped_red_labels.at<int32_t>(i, j) != p_sign_label && warped_white_mask.at<uchar>(i, j) > 0)
                    {
                        white_score += cv::min({red_px, green_px, blue_px}) / px_max_val;
                        vibe_check.at<uchar>(i, j) = std::min(cv::min({red_px, green_px, blue_px}), int(px_max_val));
                    }
                }

                if (red_px < COLOR_THRESHOLD && green_px < COLOR_THRESHOLD && blue_px < COLOR_THRESHOLD)
                {
                    black_total += (px_max_val - std::max({red_px, green_px, blue_px})) / px_max_val;
                    if (warped_red_labels.at<int32_t>(i, j) != p_sign_label && warped_black_mask.at<uchar>(i, j) > 0)
                    {
                        thin_black_score += (px_max_val - std::max({red_px, green_px, blue_px})) / px_max_val;
                        vibe_check.at<uchar>(i, j) = std::min((px_max_val - std::max({red_px, green_px, blue_px})), px_max_val);
                    }
                    if (warped_red_labels.at<int32_t>(i, j) != p_sign_label && thick_black_mask.at<uchar>(i, j) > 0)
                    {
                        thick_black_score += (px_max_val - std::max({red_px, green_px, blue_px})) / px_max_val;
                        vibe_check.at<uchar>(i, j) = std::min((px_max_val - std::max({red_px, green_px, blue_px})), px_max_val);
                    }
                }
            }
            else
            {
                outside_total ++;
                if(warped_red_labels.at<int32_t>(i, j) == p_sign_label)
                {
                    vibe_check.at<uchar>(i, j) = 155;
                    red_outside ++;
                }
            }
        }
    }
    // nice prints
    show_pic(vibe_check);

    if(red_total == 0 || white_total == 0 || black_total == 0 || red_outside/outside_total > RED_OUTSIDE_STOP_THRESHOLD)
    {
        return 0;
    }

    float_t black_score = std::max(thick_black_score, thin_black_score);
    std::cout << "white score: " << white_score / white_total << std::endl;
    std::cout << "red score: " << red_score / red_total << std::endl;
    std::cout << "black score: " << black_score / black_total << std::endl;
    // std::cout << std::endl;
    std::cout << "black total: " << black_total << " black score " << black_score << std::endl;
    std::cout << "outside ratio: " << red_outside/outside_total << std::endl;
    std::cout << "final score:" << 0.3 * (red_score/red_total) + 0.3 * (white_score/white_total) + 0.4 * (black_score / black_total) << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;

    float_t final_red = red_score/red_total;
    float_t final_white = white_score/white_total;
    float_t final_black = black_score/black_total;
    if (final_red < MIN_CHUNK_SCORE || final_white < MIN_CHUNK_SCORE || final_black < MIN_CHUNK_SCORE)
    {
        return std::min(std::min(final_red, final_black), final_white);
    } 

    return 0.3 * (red_score/red_total) + 0.3 * (white_score/white_total) + 0.4 * (black_score / black_total);
}

float check_for_crossing(cv::Mat &p_white_mask, cv::Mat &p_black_mask, crossing_chunk p_cr_chunk, 
    cv::Mat &p_label_mat, int32_t p_sign_label, cv::Mat &p_cr_template, cv::Mat &p_img)
{
    if(has_small_angle(p_cr_chunk))
    {
        return 0;
    }

    cv::Point2f bottom_left_corner = cv::Point2f(p_cr_chunk.left.x, p_cr_chunk.left.y);
    cv::Point2f bottom_right_corner = cv::Point2f(p_cr_chunk.right.x, p_cr_chunk.right.y);

    // compute top left and top right corners 
    cv::Point2f bottom_dir = bottom_right_corner - bottom_left_corner;

    float upward_length = std::sqrt(bottom_dir.x * bottom_dir.x + bottom_dir.y * bottom_dir.y);
    cv::Point2f perp(-bottom_dir.y / upward_length, bottom_dir.x / upward_length); // rotated counterclockwise

    float bounding_width = cv::norm(bottom_right_corner - bottom_left_corner);

    cv::Point2f top(p_cr_chunk.top.x, p_cr_chunk.top.y);
    cv::Point2f offset = (bottom_dir / bounding_width) * (bounding_width/ 2); // half the width along bottom edge direction
    cv::Point2f top_left_corner = top - offset;
    cv::Point2f top_right_corner = top + offset;

    std::vector<cv::Point2f> src_points = {
        top_left_corner,
        top_right_corner,
        bottom_right_corner,
        bottom_left_corner
	};

	int32_t width = p_cr_template.cols;
	int32_t height = p_cr_template.rows;
	
	std::vector<cv::Point2f> dst_points = {
		cv::Point2f(0, 0),           
		cv::Point2f(width - 1, 0),    
		cv::Point2f(width - 1, height - 1),
		cv::Point2f(0, height - 1)    
	};
	cv::Mat H = cv::getPerspectiveTransform(src_points, dst_points);

	cv::Mat warped_red_labels;
    cv::Mat warped_white_mask;
    cv::Mat warped_black_mask;

    cv::Mat float_labels;
    cv::Mat ref_img;
    p_label_mat.convertTo(float_labels, CV_32F);

	cv::warpPerspective(float_labels, warped_red_labels, H, cv::Size(width, height), cv::INTER_NEAREST );
	cv::warpPerspective(p_black_mask, warped_black_mask, H, cv::Size(width, height), cv::INTER_NEAREST );
	cv::warpPerspective(p_white_mask, warped_white_mask, H, cv::Size(width, height), cv::INTER_NEAREST );
	cv::warpPerspective(p_img, ref_img, H, cv::Size(width, height), cv::INTER_NEAREST );


    //todo remove vibe check
    cv::Mat vibe_check = warped_white_mask.clone();

    // nice prints 
    // show_pic(p_cr_template);
    // show_pic(warped_black_mask);

    // show_pic(p_cr_template);
    // show_pic(warped_red_labels);
    // show_pic(warped_white_mask);

    warped_red_labels.convertTo(warped_red_labels, CV_32S);
    
    float_t red_score = 0, red_total = 0;
    float_t white_score = 0, white_total = 0;
    float_t black_score = 0, black_total = 0;
    float_t red_outside = 0, outside_total = 0;
    double px_max_val = 255.0;

    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            vibe_check.at<uchar>(i, j) = 0;
            // if not green
            if (! ((p_cr_template.at<cv::Vec3b>(i, j)[GREEN]) > COLOR_THRESHOLD
                && (p_cr_template.at<cv::Vec3b>(i, j)[RED]) < COLOR_THRESHOLD
                && (p_cr_template.at<cv::Vec3b>(i, j)[BLUE]) < COLOR_THRESHOLD))
            {
                int32_t red_px = p_cr_template.at<cv::Vec3b>(i, j)[RED];
                int32_t green_px = p_cr_template.at<cv::Vec3b>(i, j)[GREEN];
                int32_t blue_px = p_cr_template.at<cv::Vec3b>(i, j)[BLUE];

                // score according to template
                red_total += std::max((red_px - (green_px + blue_px) / 2) / px_max_val, 0.0);
                if(warped_red_labels.at<int32_t>(i, j) == p_sign_label)
                {
                    red_score += std::max((red_px - (green_px + blue_px) / 2) / px_max_val, 0.0);
                    vibe_check.at<uchar>(i, j) = std::min(std::max((red_px - (green_px + blue_px) / 2) / px_max_val, 0.0) * px_max_val, px_max_val);
                }

                if (red_px > COLOR_THRESHOLD && green_px > COLOR_THRESHOLD && blue_px > COLOR_THRESHOLD)
                {
                    white_total += cv::min({red_px, green_px, blue_px}) / px_max_val;

                    if (warped_red_labels.at<int32_t>(i, j) != p_sign_label && warped_white_mask.at<uchar>(i, j) > 0)
                    {
                        white_score += cv::min({red_px, green_px, blue_px}) / px_max_val;
                        vibe_check.at<uchar>(i, j) = std::min(cv::min({red_px, green_px, blue_px}), int(px_max_val));
                    }
                }

                if (red_px < COLOR_THRESHOLD && green_px < COLOR_THRESHOLD && blue_px < COLOR_THRESHOLD)
                {
                    black_total += (px_max_val - std::max({red_px, green_px, blue_px})) / px_max_val;
                    if (warped_red_labels.at<int32_t>(i, j) != p_sign_label && warped_black_mask.at<uchar>(i, j) > 0)
                    {
                        black_score += (px_max_val - std::max({red_px, green_px, blue_px})) / px_max_val;
                        vibe_check.at<uchar>(i, j) = std::min((px_max_val - std::max({red_px, green_px, blue_px})), px_max_val);
                    }
                }
            }
            else
            {
                outside_total ++;
                if(warped_red_labels.at<int32_t>(i, j) == p_sign_label)
                {
                    vibe_check.at<uchar>(i, j) = 155;
                    red_outside ++;
                }
            }
        }
    }
    // nice prints
    // show_pic(vibe_check);
    // show_pic(ref_img);
    // 84, 85

    if(red_total == 0 || white_total == 0 || black_total == 0 || red_outside/outside_total > RED_OUTSIDE_STOP_THRESHOLD)
    {
        return 0;
    }

    // std::cout << "white score: " << white_score / white_total << std::endl;
    // std::cout << "red score: " << red_score / red_total << std::endl;
    // std::cout << "black score: " << black_score / black_total << std::endl;
    // // std::cout << "black total: " << black_total << " black score " << black_score << std::endl;
    // std::cout << "outside ratio: " << red_outside/outside_total << std::endl;
    // std::cout << "final score:" << 0.3 * (red_score/red_total) + 0.3 * (white_score/white_total) + 0.4 * (black_score / black_total) << std::endl;
    // std::cout << std::endl;
    // std::cout << std::endl;

    return 0.3 * (red_score/red_total) + 0.3 * (white_score/white_total) + 0.4 * (black_score / black_total);
}

// todo try soebel -> square it to make errors (<0) smaller and match with the soebel of the template