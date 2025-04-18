#include "utility.h"

double point_area(point p_point1, point p_point2, point p_point3)
{
    return abs( p_point1.x * (p_point2.y - p_point3.y) + 
                p_point2.x * (p_point3.y - p_point1.y) +
                p_point3.x * (p_point1.y - p_point2.y)  ) / 2.0;
}

double chunk_area(give_way_chunk p_chunk)
{
    return abs( p_chunk.top_left.x * (p_chunk.top_right.y - p_chunk.bottom.y) +
                p_chunk.top_right.x * (p_chunk.bottom.y - p_chunk.top_left.y) +
                p_chunk.bottom.x * (p_chunk.top_left.y - p_chunk.top_right.y)) / 2.0;
}

point scale_point_towards_centroid(point p_point, point p_centroid, double p_scale_factor)
{
    return point(p_centroid.x + p_scale_factor * (p_point.x - p_centroid.x), 
                 p_centroid.y + p_scale_factor * (p_point.y - p_centroid.y) );
}

give_way_chunk get_inner_triangle_chunk(give_way_chunk p_chunk, double p_scale)
{
    // get the centroid of the original triangle
    double centroid_x = (p_chunk.top_left.x + p_chunk.top_right.x + p_chunk.bottom.x) / 3;
    double centroid_y = (p_chunk.top_left.y + p_chunk.top_right.y + p_chunk.bottom.y) / 3;
    point centroid = point(centroid_x, centroid_y);

    // Scale the points towards the centroid
    point scaled_left = scale_point_towards_centroid(p_chunk.top_left, centroid, p_scale);
    point scaled_right = scale_point_towards_centroid(p_chunk.top_right, centroid, p_scale);
    point scaled_bottom = scale_point_towards_centroid(p_chunk.bottom, centroid, p_scale);

    return give_way_chunk(scaled_left, scaled_right, scaled_bottom);
}

float get_side(point p_point, point p_vertex_p1, point p_vertex_p2)
{
    return  (p_point.x - p_vertex_p2.x) * (p_vertex_p1.y - p_vertex_p2.y) -
            (p_vertex_p1.x - p_vertex_p2.x) * (p_point.y - p_vertex_p2.y);
}

bool point_in_triangle(point p_point, point p_point_tr_1, point p_point_tr_2, point p_point_tr_3)
{
    float d1 = get_side(p_point, p_point_tr_1, p_point_tr_2);
    float d2 = get_side(p_point, p_point_tr_2, p_point_tr_3);
    float d3 = get_side(p_point, p_point_tr_3, p_point_tr_1);

    float has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    float has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(has_neg && has_pos);
}

bool has_small_angle(give_way_chunk p_chunk)
{
    float a_side = sqrt(pow(p_chunk.top_left.x - p_chunk.top_right.x, 2) + pow(p_chunk.top_left.y - p_chunk.top_right.y, 2));
    float b_side = sqrt(pow(p_chunk.bottom.x - p_chunk.top_right.x, 2) + pow(p_chunk.bottom.y - p_chunk.top_right.y, 2));
    float c_side = sqrt(pow(p_chunk.top_left.x - p_chunk.bottom.x, 2) + pow(p_chunk.top_left.y - p_chunk.bottom.y, 2));

    if(a_side == 0 || b_side == 0 || c_side == 0)
    {
        return true;
    }

    float a_angle = acos((pow(b_side, 2) + pow(c_side, 2) - pow(a_side, 2)) / (2 * b_side * c_side)) * 180 / M_PI;
    float b_angle = acos((pow(a_side, 2) + pow(c_side, 2) - pow(b_side, 2)) / (2 * a_side * c_side)) * 180 / M_PI;
    float c_angle = acos((pow(b_side, 2) + pow(a_side, 2) - pow(c_side, 2)) / (2 * b_side * a_side)) * 180 / M_PI;

    bool min_triangle_condition = (a_angle < MIN_TRIANGLE_ANGLE || b_angle < MIN_TRIANGLE_ANGLE || c_angle < MIN_TRIANGLE_ANGLE);

    return min_triangle_condition;
}

void print_detection(cv::Mat &p_img, give_way_chunk gw_chunk, float p_score)
{
    cv::Scalar color = cv::Scalar(0, 255, 0);
    int thickness = 2;

    cv::line(p_img, gw_chunk.bottom.get_cv_point(), gw_chunk.top_left.get_cv_point(), color, thickness);
    cv::line(p_img, gw_chunk.top_left.get_cv_point(), gw_chunk.top_right.get_cv_point(), color, thickness);
    cv::line(p_img, gw_chunk.top_right.get_cv_point(), gw_chunk.bottom.get_cv_point(), color, thickness);

    char stringed_score[5];
    snprintf(stringed_score, sizeof(stringed_score), "%.1f", p_score * 100);

    cv::Point text_pt(int((gw_chunk.top_left.x + gw_chunk.top_right.x)/2), std::min(gw_chunk.top_left.y, gw_chunk.top_right.y));
    int font_size = 3;
    cv::putText(p_img, stringed_score, text_pt, 1, font_size, cv::Scalar(0, 0, 0), 8);   
    cv::putText(p_img, stringed_score, text_pt, 1, font_size, cv::Scalar(0, 255, 0), 3);   
}

void show_pic(cv::Mat p_img)
{
	cv::namedWindow("Display Image", cv::WINDOW_GUI_NORMAL); 
	cv::imshow("Display Image", p_img); 
	cv::waitKey(0); 
	cv::destroyAllWindows();
}