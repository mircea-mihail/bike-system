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

float get_angle(point A, point B, point C) {
    float BAx = A.x - B.x;
    float BAy = A.y - B.y;
    float BCx = C.x - B.x;
    float BCy = C.y - B.y;

    float dot = BAx * BCx + BAy * BCy;
    float mag_BA = std::sqrt(BAx * BAx + BAy * BAy);
    float mag_BC = std::sqrt(BCx * BCx + BCy * BCy);

    if (mag_BA == 0 || mag_BC == 0)
        return 0.0f;

    float cos_angle = dot / (mag_BA * mag_BC);
    cos_angle = std::clamp(cos_angle, -1.0f, 1.0f); 

    float angle_rad = std::acos(cos_angle);
    return angle_rad * 180.0f / M_PI;
}

bool has_small_angle(stop_chunk p_chunk)
{
    float angle = 0;
    angle = get_angle(p_chunk.top_left, p_chunk.top_right, p_chunk.right_top);
    if(angle > IDEAL_OCTOGON_ANGLE + OCTOGON_ANGLE_THRESHOLD || angle < IDEAL_OCTOGON_ANGLE - OCTOGON_ANGLE_THRESHOLD)
    {
        return true;
    }
    angle = get_angle(p_chunk.top_right, p_chunk.right_top, p_chunk.right_bottom);
    if(angle > IDEAL_OCTOGON_ANGLE + OCTOGON_ANGLE_THRESHOLD || angle < IDEAL_OCTOGON_ANGLE - OCTOGON_ANGLE_THRESHOLD)
    {
        return true;
    }
    angle = get_angle(p_chunk.right_top, p_chunk.right_bottom, p_chunk.bottom_right);
    if(angle > IDEAL_OCTOGON_ANGLE + OCTOGON_ANGLE_THRESHOLD || angle < IDEAL_OCTOGON_ANGLE - OCTOGON_ANGLE_THRESHOLD)
    {
        return true;
    }
    angle = get_angle(p_chunk.right_bottom, p_chunk.bottom_right, p_chunk.bottom_left);
    if(angle > IDEAL_OCTOGON_ANGLE + OCTOGON_ANGLE_THRESHOLD || angle < IDEAL_OCTOGON_ANGLE - OCTOGON_ANGLE_THRESHOLD)
    {
        return true;
    }
    angle = get_angle(p_chunk.bottom_right, p_chunk.bottom_left, p_chunk.left_bottom);
    if(angle > IDEAL_OCTOGON_ANGLE + OCTOGON_ANGLE_THRESHOLD || angle < IDEAL_OCTOGON_ANGLE - OCTOGON_ANGLE_THRESHOLD)
    {
        return true;
    }
    angle = get_angle(p_chunk.bottom_left, p_chunk.left_bottom, p_chunk.left_top);
    if(angle > IDEAL_OCTOGON_ANGLE + OCTOGON_ANGLE_THRESHOLD || angle < IDEAL_OCTOGON_ANGLE - OCTOGON_ANGLE_THRESHOLD)
    {
        return true;
    }
    angle = get_angle(p_chunk.left_bottom, p_chunk.left_top, p_chunk.top_left);
    if(angle > IDEAL_OCTOGON_ANGLE + OCTOGON_ANGLE_THRESHOLD || angle < IDEAL_OCTOGON_ANGLE - OCTOGON_ANGLE_THRESHOLD)
    {
        return true;
    }
    angle = get_angle(p_chunk.left_top, p_chunk.top_left, p_chunk.top_right);
    if(angle > IDEAL_OCTOGON_ANGLE + OCTOGON_ANGLE_THRESHOLD || angle < IDEAL_OCTOGON_ANGLE - OCTOGON_ANGLE_THRESHOLD)
    {
        return true;
    }
    return false;
}

bool has_small_angle(no_bikes_chunk p_chunk)
{
    float angle = 0;
    angle = get_angle(p_chunk.top, p_chunk.right, p_chunk.bottom);
    if(angle > IDEAL_SQUARE_ANGLE + SQUARE_ANGLE_THRESHOLD || angle < IDEAL_SQUARE_ANGLE - SQUARE_ANGLE_THRESHOLD)
    {
        return true;
    }
    angle = get_angle(p_chunk.right, p_chunk.bottom, p_chunk.left);
    if(angle > IDEAL_SQUARE_ANGLE + SQUARE_ANGLE_THRESHOLD || angle < IDEAL_SQUARE_ANGLE - SQUARE_ANGLE_THRESHOLD)
    {
        return true;
    }
    angle = get_angle(p_chunk.bottom, p_chunk.left, p_chunk.top);
    if(angle > IDEAL_SQUARE_ANGLE + SQUARE_ANGLE_THRESHOLD || angle < IDEAL_SQUARE_ANGLE - SQUARE_ANGLE_THRESHOLD)
    {
        return true;
    }
    angle = get_angle(p_chunk.left, p_chunk.top, p_chunk.right);
    if(angle > IDEAL_SQUARE_ANGLE + SQUARE_ANGLE_THRESHOLD || angle < IDEAL_SQUARE_ANGLE - SQUARE_ANGLE_THRESHOLD)
    {
        return true;
    }
    
    return false;
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

bool has_small_angle(crossing_chunk p_chunk)
{
    float a_side = sqrt(pow(p_chunk.left.x - p_chunk.right.x, 2) + pow(p_chunk.left.y - p_chunk.right.y, 2));
    float b_side = sqrt(pow(p_chunk.top.x - p_chunk.right.x, 2) + pow(p_chunk.top.y - p_chunk.right.y, 2));
    float c_side = sqrt(pow(p_chunk.left.x - p_chunk.top.x, 2) + pow(p_chunk.left.y - p_chunk.top.y, 2));

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

void print_give_way(cv::Mat &p_img, give_way_chunk gw_chunk, float p_score)
{
    cv::Scalar color = cv::Scalar(0, 255, 0);
    int thickness = DRAW_LINE_THICKNESS;

    cv::line(p_img, gw_chunk.bottom.get_cv_point(), gw_chunk.top_left.get_cv_point(), color, thickness);
    cv::line(p_img, gw_chunk.top_left.get_cv_point(), gw_chunk.top_right.get_cv_point(), color, thickness);
    cv::line(p_img, gw_chunk.top_right.get_cv_point(), gw_chunk.bottom.get_cv_point(), color, thickness);

    char stringed_score[30] = "give-way ";
    char buf[10];
    snprintf(buf, sizeof(buf), "%.1f", p_score * 100);
    strcat(stringed_score, buf);


    cv::Point text_pt(gw_chunk.top_left.x, std::min(gw_chunk.top_left.y, gw_chunk.top_right.y));
    int font_size = 3;
    cv::putText(p_img, stringed_score, text_pt, 1, font_size, cv::Scalar(0, 0, 0), 8);   
    cv::putText(p_img, stringed_score, text_pt, 1, font_size, cv::Scalar(0, 255, 0), 3);   
}

void print_stop(cv::Mat &p_img, stop_chunk p_st_chunk, float p_score)
{
    cv::Scalar color = cv::Scalar(0, 255, 0);
    int thickness = DRAW_LINE_THICKNESS;

    cv::line(p_img, cv::Point2d(p_st_chunk.top_left.x, p_st_chunk.top_left.y), 
        cv::Point2d(p_st_chunk.top_right.x, p_st_chunk.top_right.y), color, thickness);
    cv::line(p_img, cv::Point2d(p_st_chunk.top_left.x, p_st_chunk.top_left.y), 
        cv::Point2d(p_st_chunk.left_top.x, p_st_chunk.left_top.y), color, thickness);
    cv::line(p_img, cv::Point2d(p_st_chunk.bottom_left.x, p_st_chunk.bottom_left.y),
        cv::Point2d(p_st_chunk.bottom_right.x, p_st_chunk.bottom_right.y), color, thickness);
    cv::line(p_img, cv::Point2d(p_st_chunk.bottom_left.x, p_st_chunk.bottom_left.y),
        cv::Point2d(p_st_chunk.left_bottom.x, p_st_chunk.left_bottom.y), color, thickness);
    cv::line(p_img, cv::Point2d(p_st_chunk.left_bottom.x, p_st_chunk.left_bottom.y),
        cv::Point2d(p_st_chunk.left_top.x, p_st_chunk.left_top.y), color, thickness);
    cv::line(p_img, cv::Point2d(p_st_chunk.right_bottom.x, p_st_chunk.right_bottom.y),
        cv::Point2d(p_st_chunk.right_top.x, p_st_chunk.right_top.y), color, thickness);
    cv::line(p_img, cv::Point2d(p_st_chunk.top_right.x, p_st_chunk.top_right.y),
        cv::Point2d(p_st_chunk.right_top.x, p_st_chunk.right_top.y), color, thickness);
    cv::line(p_img, cv::Point2d(p_st_chunk.right_bottom.x, p_st_chunk.right_bottom.y),
        cv::Point2d(p_st_chunk.bottom_right.x, p_st_chunk.bottom_right.y), color, thickness);

    char stringed_score[30] = "stop ";
    char buf[10];
    snprintf(buf, sizeof(buf), "%.1f", p_score * 100);
    strcat(stringed_score, buf);

    cv::Point text_pt(p_st_chunk.top_left.x, std::min(p_st_chunk.top_left.y, p_st_chunk.top_right.y));
    int font_size = 3;
    cv::putText(p_img, stringed_score, text_pt, 1, font_size, cv::Scalar(0, 0, 0), 8);   
    cv::putText(p_img, stringed_score, text_pt, 1, font_size, cv::Scalar(0, 255, 0), 3);   
}

void print_no_bikes(cv::Mat &p_img, no_bikes_chunk nb_chunk, float p_score)
{
    cv::Scalar color = cv::Scalar(0, 255, 0);
    int thickness = DRAW_LINE_THICKNESS;

    float centerX = (nb_chunk.left.x + nb_chunk.right.x) / 2.0;
    float centerY = (nb_chunk.top.y + nb_chunk.bottom.y) / 2.0;

    cv::Point2f center(centerX, centerY);

    float axisX = (nb_chunk.right.x - nb_chunk.left.x) / 2.0;
    float axisY = (nb_chunk.bottom.y - nb_chunk.top.y) / 2.0;

    cv::ellipse(p_img, center, cv::Size(axisX, axisY), 0, 0, 360, cv::Scalar(0, 255, 0), thickness);

    char stringed_score[30] = "no-bikes ";
    char buf[10];
    snprintf(buf, sizeof(buf), "%.1f", p_score * 100);
    strcat(stringed_score, buf);

    cv::Point text_pt(nb_chunk.left.x, nb_chunk.top.y);
    int font_size = 3;
    cv::putText(p_img, stringed_score, text_pt, 1, font_size, cv::Scalar(0, 0, 0), 8);   
    cv::putText(p_img, stringed_score, text_pt, 1, font_size, cv::Scalar(0, 255, 0), 3);   
}

void print_crossing(cv::Mat &p_img, crossing_chunk cr_chunk, float p_score)
{
    cv::Scalar color = cv::Scalar(0, 255, 0);
    int thickness = DRAW_LINE_THICKNESS;

    cv::line(p_img, cr_chunk.top.get_cv_point(), cr_chunk.left.get_cv_point(), color, thickness);
    cv::line(p_img, cr_chunk.left.get_cv_point(), cr_chunk.right.get_cv_point(), color, thickness);
    cv::line(p_img, cr_chunk.right.get_cv_point(), cr_chunk.top.get_cv_point(), color, thickness);

    char stringed_score[30] = "crossing ";
    char buf[10];
    snprintf(buf, sizeof(buf), "%.1f", p_score * 100);
    strcat(stringed_score, buf);

    cv::Point text_pt(cr_chunk.left.x, cr_chunk.top.y);
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

cv::Point2f get_line_intersection( point A, point B, point C, point D) {
    float a1 = B.y - A.y;
    float b1 = A.x - B.x;
    float c1 = a1 * A.x + b1 * A.y;

    float a2 = D.y - C.y;
    float b2 = C.x - D.x;
    float c2 = a2 * C.x + b2 * C.y;

    float determinant = a1 * b2 - a2 * b1;

    if (determinant == 0) {
        return cv::Point2f(0, 0);
    } else {
        float x = (b2 * c1 - b1 * c2) / determinant;
        float y = (a1 * c2 - a2 * c1) / determinant;
        return cv::Point2f(x, y);
    }
}

void print_bounding_box(cv::Mat &p_img, int32_t p_x, int32_t p_y, int32_t p_w, int32_t p_h)
{
    // print a bounding box above all checked shapes
    cv::Scalar color(255,0,0);
    cv::Rect rect(p_x, p_y, p_w, p_h);
    uint8_t thickness = DRAW_LINE_THICKNESS;
    cv::rectangle(p_img, rect, color, thickness);
}