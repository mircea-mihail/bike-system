from collections import deque

from utility import *
from visualisation import *

def check_for_gw(p_hsv_img, p_chunk, p_label_mat):
    # filter out triangles with angle smaller than minimum
    if has_small_angle(p_chunk):
        return 0

    px_outside_gw = 0
    red_outside_gw = 0

    thin_red_score = 0
    thin_white_score = 0
    thick_red_score = 0
    thick_white_score = 0

    pixels_checked = 0
    found_triangle_in_line = False

    view_win_top_left = point(min(p_chunk.top_left_point.x, p_chunk.bottom_point.x), min(p_chunk.top_left_point.y, p_chunk.top_right_point.y))
    view_win_btm_right = point(max(p_chunk.top_right_point.x, p_chunk.bottom_point.x), p_chunk.bottom_point.y)

    thin_inner_triangle = get_inner_triangle_chunk(p_chunk, THIN_GW_BORDER_PER_CENT) 
    thick_inner_triangle = get_inner_triangle_chunk(p_chunk, THICK_GW_BORDER_PER_CENT) 

    # Draw the smaller triangle
    for i in range(int(view_win_top_left.y), int(view_win_btm_right.y)):
        found_triangle_in_line = False

        for j in range(int(view_win_top_left.x), int(view_win_btm_right.x)):
            if point_in_triangle(point(j, i), p_chunk.top_left_point, p_chunk.top_right_point, p_chunk.bottom_point):
                found_triangle_in_line = True
                pixels_checked += 1

                # check for thin border
                if point_in_triangle(point(j, i), thin_inner_triangle.top_left_point, thin_inner_triangle.top_right_point, thin_inner_triangle.bottom_point):
                    if is_white(p_hsv_img[i][j]) and (p_label_mat[i][j] == 0):
                        thin_white_score += 1
                else:
                    if p_label_mat[i][j] != 0:
                        thin_red_score += 1

                # and check for thick border
                if point_in_triangle(point(j, i), thick_inner_triangle.top_left_point, thick_inner_triangle.top_right_point, thick_inner_triangle.bottom_point):
                    if is_white(p_hsv_img[i][j]) and (p_label_mat[i][j] == 0):
                        thick_white_score += 1
                else:
                    if p_label_mat[i][j] != 0:
                        thick_red_score += 1
            # if not in triangle but red
            else: 
                px_outside_gw += 1
                if is_red(p_hsv_img[i][j]):
                    red_outside_gw += 1

            # if found triangle in line and then no longer triangle, to the end of line will not find triangle again 
            # elif found_triangle_in_line:
            #     break
            
    # if there are a lot of red pixels outside of triangle, likely not a give way
    if red_outside_gw / px_outside_gw > RED_OUTSIDE_GW_THRESHOLD:
        # print("too much red outside:", red_outside_gw/px_outside_gw)
        return 0
    
    #get final score for thin
    thin_white_area = chunk_area(thin_inner_triangle)
    thin_red_area = (chunk_area(p_chunk) - thin_white_area) 
    thin_white_checked = thin_white_area / (thin_white_area + thin_red_area) * pixels_checked
    thin_red_checked = pixels_checked - thin_white_checked
    final_thin_score = (min(thin_white_score / thin_white_checked, 1) + min(thin_red_score / thin_red_checked, 1)) / 2

    #get final score for thick 
    thick_white_area = chunk_area(thick_inner_triangle)
    thick_white_area = chunk_area(thick_inner_triangle)
    thick_red_area = (chunk_area(p_chunk) - thick_white_area) 
    thick_white_checked = thick_white_area / (thick_white_area + thick_red_area) * pixels_checked
    thick_red_checked = pixels_checked - thick_white_checked
    final_thick_score = (min(thick_white_score / thick_white_checked, 1) + min(thick_red_score / thick_red_checked, 1)) / 2

    # if final_thin_score > final_thick_score:
    #     print("outer:", thin_white_score/thin_white_checked, "inner", thin_red_score/thin_red_checked) 
    # else:
    #     print("outer:", thick_white_score/thick_white_checked, "inner", thick_red_score/thick_red_checked) 

    return max(final_thin_score, final_thick_score)

def fill_in_shape(p_img, p_label_mat, p_hsv_img, p_label, p_x, p_y):
    highest_point_in_sign = point(len(p_hsv_img[0]-1), len(p_hsv_img-1))
    gw_chunk = give_way_chunk(point(p_x, p_y), point(p_x, p_y), point(p_x, p_y))

    chunk_size = 1 
    p_fronteer = deque([]) 

    p_fronteer.append((p_y, p_x))

    current_node = 0

    while len(p_fronteer) > 0:
    # while len(p_fronteer) > current_node:
        curr_y = int(p_fronteer[current_node][0])
        curr_x = int(p_fronteer[current_node][1])

        if not(curr_y >= len(p_img) - 1 or curr_x >= len(p_img[0]) - 1 or curr_y <= 1 or curr_x <= 1):
            p_label_mat[curr_y][curr_x] = p_label
            for i in range(-1, 2):
                for j in range(-1, 2):
                    if not((i == 0 and j == 0) or (i != 0 and j != 0)):
                        if (is_red(p_hsv_img[curr_y + i][curr_x + j]) 
                            or check_color_variance(p_img[curr_y][curr_x], p_img[curr_y + i][curr_x + j]))\
                            and p_label_mat[curr_y + i][curr_x + j] == 0:

                            p_label_mat[curr_y + i][curr_x + j] = p_label

                            p_fronteer.append((curr_y + i, curr_x + j))
                            chunk_size += 1

                            if(curr_x + j < gw_chunk.top_left_point.x):
                                gw_chunk.top_left_point.y = curr_y + i
                                gw_chunk.top_left_point.x = curr_x + j 

                            if(curr_x + j > gw_chunk.top_right_point.x):
                                gw_chunk.top_right_point.y = curr_y + i
                                gw_chunk.top_right_point.x = curr_x + j

                            if(curr_y + i > gw_chunk.bottom_point.y):
                                gw_chunk.bottom_point.y = curr_y + i
                                gw_chunk.bottom_point.x = curr_x + j

                            if(curr_y + i < highest_point_in_sign.y):
                                highest_point_in_sign.x = curr_x + i
                                highest_point_in_sign.y = curr_y + j 


        # current_node += 1 
        p_fronteer.popleft()

    sign_height = gw_chunk.bottom_point.y - (gw_chunk.top_left_point.y + gw_chunk.top_right_point.y)/2
    allowed_y_position = min(gw_chunk.top_left_point.y, gw_chunk.top_right_point.y) - RED_ABOVE_SIGN * sign_height
    
    if chunk_size >= MIN_CHUNK_SIZE and highest_point_in_sign.y > allowed_y_position:
        chunk_score = check_for_gw(p_hsv_img, gw_chunk, p_label_mat)
        if chunk_score > MIN_CHUNK_SCORE:
            # print("similitude:", chunk_score)
            return chunk_score, gw_chunk
    
    return 0, 0


def detect_gw(p_img):
    hsv_image = cv.cvtColor(p_img, cv.COLOR_BGR2HSV)
    label_mat = np.zeros((len(p_img), len(p_img[0])))

    total_labels = 3 
    starting_label = 1 
    current_label = starting_label
    
    for i in range(1, len(p_img) - 2):
        for j in range(1, len(p_img[0]) - 2):
            if is_red(hsv_image[i][j]) and label_mat[i][j] == 0:
                current_score, gw_chunk = fill_in_shape(p_img, label_mat, hsv_image, current_label, j, i)

                # if found a sign
                if(current_score > 0):
                    return current_score, gw_chunk

                current_label += 1
                if current_label > total_labels:
                    current_label = starting_label

    # second for print
    return 0
