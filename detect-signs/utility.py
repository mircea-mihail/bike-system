import numpy as np
import cv2 as cv

from my_constants import *


def is_red(p_hsv_px):
    if (p_hsv_px[HUE] < ALLOWED_RED_HUE_OFFSET or p_hsv_px[HUE] > MAX_HUE - ALLOWED_RED_HUE_OFFSET):
        # dark red
        if p_hsv_px[VALUE] < VALUE_DELIMITER:
            if  p_hsv_px[VALUE] > DARK_MIN_RED_VALUE\
            and p_hsv_px[SATURATION] > DARK_MIN_RED_SATURATION:
                return True
    
        # bright red
        else:
            if p_hsv_px[SATURATION] > BRIGHT_MIN_RED_SATURATION:
                return True
    return False
                

def is_white(p_hsv_px):

    if p_hsv_px[2] > MIN_WHITE_BRIGHTNESS \
        and p_hsv_px[1] < MAX_WHITE_SATURATION:
        return True
    
    return False

def point_area(p_point1, p_point2, p_point3):
    return np.abs(p_point1.x*(p_point2.y-p_point3.y) + p_point2.x*(p_point3.y-p_point1.y) + p_point3.x*(p_point1.y-p_point2.y)) / 2.0

def chunk_area(p_chunk):
    return np.abs(p_chunk.top_left_point.x*(p_chunk.top_right_point.y-p_chunk.bottom_point.y) + p_chunk.top_right_point.x*(p_chunk.bottom_point.y-p_chunk.top_left_point.y) + p_chunk.bottom_point.x*(p_chunk.top_left_point.y-p_chunk.top_right_point.y)) / 2.0


def scale_point_towards_centroid(p_point, p_centroid, p_scale_factor):
    return point(
        p_centroid.x + p_scale_factor * (p_point.x - p_centroid.x), 
        p_centroid.y + p_scale_factor * (p_point.y - p_centroid.y)
    )

def get_inner_triangle_chunk(p_chunk, p_scale):
    # Calculate the centroid of the original triangle
    centroid_x = (p_chunk.top_left_point.x + p_chunk.top_right_point.x + p_chunk.bottom_point.x) / 3
    centroid_y = (p_chunk.top_left_point.y + p_chunk.top_right_point.y + p_chunk.bottom_point.y) / 3
    centroid = point(centroid_x, centroid_y)

    # Scale the points towards the centroid
    scaled_left_point = scale_point_towards_centroid(p_chunk.top_left_point, centroid, p_scale)
    scaled_right_point = scale_point_towards_centroid(p_chunk.top_right_point, centroid, p_scale)
    scaled_bottom_point = scale_point_towards_centroid(p_chunk.bottom_point, centroid, p_scale)

    return give_way_chunk(scaled_left_point, scaled_right_point, scaled_bottom_point)

# gets what side the point is on in relation to a line
def get_side(p_point, p_vertex_p1, p_vertex_p2):
    return (p_point.x - p_vertex_p2.x) * (p_vertex_p1.y - p_vertex_p2.y) - (p_vertex_p1.x - p_vertex_p2.x) * (p_point.y - p_vertex_p2.y)

def point_in_triangle(p_point, p_point_tr_1, p_point_tr_2, p_point_tr_3):
    d1 = get_side(p_point, p_point_tr_1, p_point_tr_2)
    d2 = get_side(p_point, p_point_tr_2, p_point_tr_3)
    d3 = get_side(p_point, p_point_tr_3, p_point_tr_1)

    has_neg = (d1 < 0) or (d2 < 0) or (d3 < 0)
    has_pos = (d1 > 0) or (d2 > 0) or (d3 > 0)

    return not(has_neg and has_pos)

# try to find a condition to filter based on shape
def has_small_angle(p_chunk):
    a_side = np.sqrt(pow(p_chunk.top_left_point.x - p_chunk.top_right_point.x, 2) + pow(p_chunk.top_left_point.y - p_chunk.top_right_point.y, 2))
    b_side = np.sqrt(pow(p_chunk.bottom_point.x - p_chunk.top_right_point.x, 2) + pow(p_chunk.bottom_point.y - p_chunk.top_right_point.y, 2))
    c_side = np.sqrt(pow(p_chunk.top_left_point.x - p_chunk.bottom_point.x, 2) + pow(p_chunk.top_left_point.y - p_chunk.bottom_point.y, 2))

    if a_side == 0 or b_side == 0 or c_side == 0:
        return True

    a_angle = np.arccos((pow(b_side, 2) + pow(c_side, 2) - pow(a_side, 2)) / (2 * b_side * c_side)) * 180 / np.pi
    b_angle = np.arccos((pow(a_side, 2) + pow(c_side, 2) - pow(b_side, 2)) / (2 * a_side * c_side)) * 180 / np.pi
    c_angle = np.arccos((pow(b_side, 2) + pow(a_side, 2) - pow(c_side, 2)) / (2 * b_side * a_side)) * 180 / np.pi

    min_triangle_condition = (a_angle < MIN_TRIANGLE_ANGLE or b_angle < MIN_TRIANGLE_ANGLE or c_angle < MIN_TRIANGLE_ANGLE)

    return min_triangle_condition

def check_color_variance(p_pixel1, p_pixel2):
    variance = 0

    for i in range(0, len(p_pixel1)):
        variance += abs(int(p_pixel1[i]) - int(p_pixel2[i]))
    
    return (variance < MAX_CLUMPING_VARIANCE)