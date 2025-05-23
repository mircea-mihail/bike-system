#include "find_signs_in_chunk.h"

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
            print_give_way(p_img, gw_chunk, chunk_score);
        #endif

        return chunk_score;
    }

    return 0;
}

float find_stop_in_chunk(cv::Mat &p_img, cv::Mat &p_white_mask, cv::Mat &p_labels, 
    cv::Mat &p_stats, int32_t p_label, std::vector<cv::Mat> &p_templates)
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
 
    float_t bias = 0.5;
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
    stop_chunk st_chunk  = stop_chunk(top_left, top_right, right_top, right_bottom, bottom_right, bottom_left, left_bottom, left_top);

    float chunk_score = check_for_stop(p_white_mask, st_chunk, p_labels, p_label, p_templates[STOP_POSITION]);
    if(chunk_score > MIN_CHUNK_SCORE)
    {
        #ifdef PRINT_STATS
            print_stop(p_img, st_chunk, chunk_score);
        #endif

        return chunk_score;
    }

    return 0;
}

float find_no_bikes_in_chunk(cv::Mat &p_img, cv::Mat &p_white_mask, cv::Mat &p_black_mask,
    cv::Mat &p_labels, cv::Mat &p_stats, int32_t p_label, std::vector<cv::Mat> &p_templates)
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

    point top_pt;
    for (int j = x; j < x + w; j++)
    {
        if (p_labels.at<int32_t>(y, j) == p_label)
        {
            top_pt.x = j;
            top_pt.y = y;
            break;
        }
    }
    no_bikes_chunk nb_chunk = no_bikes_chunk(top_pt, bottom_pt, left_pt, right_pt);

    print_no_bikes(p_img, nb_chunk, 0.8);
    show_pic(p_img);

    // float chunk_score = check_for_gw_cv(p_white_mask, gw_chunk, p_labels, p_label);
    // if(chunk_score > MIN_CHUNK_SCORE)
    // {
    //     #ifdef PRINT_STATS
    //         print_give_way(p_img, gw_chunk, chunk_score);
    //     #endif

    //     return chunk_score;
    // }

    return 0;
}