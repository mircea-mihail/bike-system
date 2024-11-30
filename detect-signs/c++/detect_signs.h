#ifndef DETECT_SIGNS_H
#define DETECT_SIGNS_H

#include <opencv2/opencv.hpp> 
#include <deque>
#include <fstream>

#include "constants.h"
#include "utility.h"
#include "memory_pool_list.h"

// iau din coltul din stanga sus labelul formei curente si verific cu asta la is red
float check_for_gw(cv::Mat &p_hsv_img, give_way_chunk p_chunk, std::array<std::array<int, IMAGE_WIDTH>, IMAGE_HEIGHT> &p_label_mat)
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
            if (point_in_triangle(point(j, i), p_chunk.top_left, p_chunk.top_right, p_chunk.bottom))
            {
                found_triangle_in_line = true;
                pixels_checked += 1;

                // check for thin border
                if (point_in_triangle(point(j, i), thin_inner_triangle.top_left, thin_inner_triangle.top_right, thin_inner_triangle.bottom))
                {
                    cv::Vec3b px = p_hsv_img.at<cv::Vec3b>(i, j);

                    if (is_white(px[HUE], px[SATURATION], px[VALUE]) && (p_label_mat[i][j] == 0))
                    {
                        thin_white_score += 1;
                    }
                }
                else
                {
                    if (p_label_mat[i][j] != 0)
                    {
                        thin_red_score += 1;
                    }
                }

                // and check for thick border
                if (point_in_triangle(point(j, i), thick_inner_triangle.top_left, thick_inner_triangle.top_right, thick_inner_triangle.bottom))
                {
                    cv::Vec3b px = p_hsv_img.at<cv::Vec3b>(i, j);

                    if (is_white(px[HUE], px[SATURATION], px[VALUE]) && (p_label_mat[i][j] == 0))
                    { 
                        thick_white_score += 1;
                    }
                }
                else
                {
                    if (p_label_mat[i][j] != 0)
                    {
                        thick_red_score += 1;
                    }
                }
            }
            // if not in triangle but red
            else 
            {
                cv::Vec3b px = p_hsv_img.at<cv::Vec3b>(i, j);

                if (is_red(px[HUE], px[SATURATION], px[VALUE]))
                {
                    red_outside_gw += 1;
                }

                px_outside_gw += 1;
            }
        }
    }

    // if there are a lot of red pixels outside of triangle, likely not a give way
    if (red_outside_gw / (double)px_outside_gw > RED_OUTSIDE_GW_THRESHOLD)
    {
        // too much red outside
        return 0;
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

void print_detection(cv::Mat &p_img, give_way_chunk gw_chunk)
{
    cv::Scalar color = cv::Scalar(0, 255, 0);
    int thickness = 5;

    cv::line(p_img, gw_chunk.bottom.get_cv_point(), gw_chunk.top_left.get_cv_point(), color, thickness);
    cv::line(p_img, gw_chunk.top_left.get_cv_point(), gw_chunk.top_right.get_cv_point(), color, thickness);
    cv::line(p_img, gw_chunk.top_right.get_cv_point(), gw_chunk.bottom.get_cv_point(), color, thickness);
}

// metoda de expansiune de la cursul de IA
float fill_in_shape(cv::Mat &p_img, std::array<std::array<int, IMAGE_WIDTH>, IMAGE_HEIGHT> &p_label_mat, cv::Mat &p_hsv_img, int32_t p_label, double p_x, double p_y)
{
    point highest_point_in_sign = point(p_hsv_img.cols, p_hsv_img.rows);
    give_way_chunk gw_chunk = give_way_chunk(point(p_x, p_y), point(p_x, p_y), point(p_x, p_y));

    int32_t chunk_size = 1;
    mp_list p_fronteer;

    p_fronteer.push_front(point(p_x, p_y));

    while (!p_fronteer.empty())
    {
        point px = p_fronteer.back();

        if ( !(px.y >= (p_img.rows - 1) || px.x >= (p_img.cols - 1) || px.y <= 1 || px.x <= 1))
        {
            p_label_mat[px.y][px.x] = p_label;
            for(int i = -1; i <= 1; i++)
            {
                for (int j = -1; j <= 1; j++)
                {
                    // only want left right top bottom 
                    if (! ((i == 0 && j == 0) || (i != 0 && j != 0)))
                    {
                        if ((is_red(p_hsv_img.at<cv::Vec3b>(px.y + i, px.x + j)) 
                            || check_color_variance(p_img.at<cv::Vec3b>(px.y, px.x), p_img.at<cv::Vec3b>(px.y + i,px.x + j)))
                            && p_label_mat[px.y + i][px.x + j] == 0)
                        {
                            p_label_mat[px.y + i][px.x + j] = p_label;

                            p_fronteer.push_front(point(px.x + j, px.y + i));
                            chunk_size += 1;

                            if(px.x + j < gw_chunk.top_left.x)
                            {
                                gw_chunk.top_left.y = px.y + i;
                                gw_chunk.top_left.x = px.x + j;
                            }

                            if(px.x + j > gw_chunk.top_right.x){
                                gw_chunk.top_right.y = px.y + i;
                                gw_chunk.top_right.x = px.x + j;
                            }

                            if(px.y + i > gw_chunk.bottom.y)
                            {
                                gw_chunk.bottom.y = px.y + i;
                                gw_chunk.bottom.x = px.x + j;
                            }

                            if(px.y + i < highest_point_in_sign.y)
                            {
                                highest_point_in_sign.x = px.x + i;
                                highest_point_in_sign.y = px.y + j;
                            }
                       }
                    }
                }
            }
        }

        p_fronteer.pop_back();
    }

    double sign_height = gw_chunk.bottom.y - (gw_chunk.top_left.y + gw_chunk.top_right.y) / 2;
    double allowed_y_position = std::min(gw_chunk.top_left.y, gw_chunk.top_right.y) - RED_ABOVE_SIGN * sign_height;
    
    if (chunk_size >= MIN_CHUNK_SIZE && highest_point_in_sign.y > allowed_y_position)
    {
        float chunk_score = check_for_gw(p_hsv_img, gw_chunk, p_label_mat);
        if(chunk_score > MIN_CHUNK_SCORE)
        {
            print_detection(p_img, gw_chunk);
            return chunk_score;
        }
    }
    
    return 0;
}

uint32_t detect_gw(cv::Mat &p_img)
{
    cv::Mat hsv_image;
    cv::cvtColor(p_img, hsv_image, cv::COLOR_BGR2HSV);

    std::array<std::array<int, IMAGE_WIDTH>, IMAGE_HEIGHT> label_mat;

    for(int i = 0; i < IMAGE_HEIGHT; i ++)
    {
        for (int j = 0; j < IMAGE_WIDTH; j++)
        {
            label_mat[i][j] = 0;
        }
    }
    float max_score = 0;

    int32_t num_detections = 0;
    int32_t total_labels = 3;
    int32_t starting_label = 1;
    int32_t current_label = starting_label;
    
    for (int i = 1; i < IMAGE_HEIGHT - 2; i++)
    {
        for (int j = 1; j < IMAGE_WIDTH - 2; j++)
        {
            if (is_red(hsv_image.at<cv::Vec3b>(i, j)) && label_mat[i][j] == 0)
            {
                float current_score = fill_in_shape(p_img, label_mat, hsv_image, current_label, j, i);

                if(current_score > 0)
                {
                    if(current_score > max_score)
                    {
                        max_score = current_score;
                    }
                    num_detections += 1;
                }

                current_label += 1;
                if(current_label > total_labels)
                {
                    current_label = starting_label;
                }
            }
        }
    }

    std::cout << "max gw score: " << max_score << std::endl;
    
    return num_detections;
}

#endif