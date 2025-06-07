import numpy as np
import os
import cv2 as cv
from my_constants import *
from matplotlib import pyplot as plt 
import time

from utility import *

SIGN_MARK_COLOR = '#FF00FF'

def view_hsv_pixel(p_hue, p_saturation, p_value):
    rgb_px = np.uint8([[[p_hue, p_saturation, p_value]]])

    hsv_px = cv.cvtColor(rgb_px, cv.COLOR_RGB2HSV)
    np.shape(hsv_px)

    hsv_px[0][0][HUE] = p_hue 
    hsv_px[0][0][SATURATION] = p_saturation 
    hsv_px[0][0][VALUE] = p_value

    rgb_px = cv.cvtColor(hsv_px, cv.COLOR_HSV2RGB)

    plt.imsave(os.path.join(IMAGE_DIR, "0-hsv-value.png"), rgb_px)
    plt.close('all')


def view_red_in_pictures(p_gwImages, p_imagesToShow):
    # for img_idx in range(len(gw_images)):
    # img_idx = CUR_IMG 
    for img_idx in p_imagesToShow: 
        start = time.time()
        img = p_gwImages[img_idx].copy()
        # img = cv.medianBlur(img, 3)
        hsv_img = cv.cvtColor(img, cv.COLOR_RGB2HSV)

        for i in range(len(hsv_img)):
            for j in range(len(hsv_img[0])):
                if is_red(hsv_img[i][j]):
                    img[i][j][0] = 255
                    img[i][j][1] = 0
                    img[i][j][2] = 0

        
        print("took ", time.time()- start, "to find only red")
                    
        plt.figure(dpi = 150)

        plt.subplot(1, 2, 1)
        plt.imshow(p_gwImages[img_idx])
        plt.axis('off')

        plt.subplot(1, 2, 2)
        plt.axis('off')
        plt.imshow(img)

        plt.savefig(os.path.join(IMAGE_DIR, "1-"+ str(img_idx) + "-popping-red.png"))
        plt.close('all')

def view_white_in_pictures(p_gwImages, p_imagesToShow):
    # for img_idx in range(len(gw_images)):
    # img_idx = CUR_IMG 
    for img_idx in p_imagesToShow: 
        img = p_gwImages[img_idx].copy()
        # img = cv.medianBlur(img, 3)
        hsv_img = cv.cvtColor(img, cv.COLOR_RGB2HSV)

        for i in range(len(hsv_img)):
            for j in range(len(hsv_img[0])):
                if is_white(hsv_img[i][j]):
                    img[i][j][0] = 255
                    img[i][j][1] = 255
                    img[i][j][2] = 255
                    
        plt.figure(dpi = 150)
        plt.subplot(1, 2, 1)
        plt.imshow(p_gwImages[img_idx])
        plt.axis('off')

        plt.subplot(1, 2, 2)
        plt.axis('off')
        plt.imshow(img)

        plt.savefig(os.path.join(IMAGE_DIR, "2-"+ str(img_idx) + "-popping-white.png"))
        plt.close('all')



# draw the gw sign to have a better idea about how the template looks

def drwa_gw(p_img, p_chunk):
    draw_percent = THICK_GW_BORDER_PER_CENT
    # filter out triangles with angle smaller than minimum
    if has_small_angle(p_chunk):
        print('bad trianlge')

    found_triangle_in_line = False

    view_win_top_left = point(min(p_chunk.top_left_point.x, p_chunk.bottom_point.x), min(p_chunk.top_left_point.y, p_chunk.top_right_point.y))
    view_win_btm_right = point(max(p_chunk.top_right_point.x, p_chunk.bottom_point.x), p_chunk.bottom_point.y)

    inner_triangle = get_inner_triangle_chunk(p_chunk, draw_percent) 

    # Draw the smaller triangle
    for i in range(int(view_win_top_left.y), int(view_win_btm_right.y)):
        found_triangle_in_line = False

        for j in range(int(view_win_top_left.x), int(view_win_btm_right.x)):
            if point_in_triangle(point(j, i), p_chunk.top_left_point, p_chunk.top_right_point, p_chunk.bottom_point):
                found_triangle_in_line = True

                if point_in_triangle(point(j, i), inner_triangle.top_left_point, inner_triangle.top_right_point, inner_triangle.bottom_point):
                    p_img[i][j][RED_PX] = 255
                    p_img[i][j][GREEN_PX] = 255
                    p_img[i][j][BLUE_PX] = 255

                else:
                    p_img[i][j][RED_PX] = 255
                    p_img[i][j][GREEN_PX] = 0
                    p_img[i][j][BLUE_PX] = 0

            # if found triangle in line and then no longer triangle, to the end of line will not find triangle again 
            elif found_triangle_in_line:
                break

def whiteout_img(p_img):
    for i in range(0, len(p_img)):
        for j in range(0, len(p_img[0])):
            p_img[i][j][RED_PX] = 255
            p_img[i][j][BLUE_PX] = 255
            p_img[i][j][GREEN_PX] = 255

def plot_triangle(p_img, p_chunk, p_score):
    # very unclean code for this one but it gets the job done
    plt.subplot(1, 3, 2)
    plt.imshow(p_img)
    plt.axis('off')

    plt.plot([p_chunk.top_left_point.x, p_chunk.top_right_point.x], 
             [p_chunk.top_left_point.y, p_chunk.top_right_point.y],
             color=SIGN_MARK_COLOR, linewidth=0.3)

    plt.plot([p_chunk.top_left_point.x, p_chunk.bottom_point.x],
             [p_chunk.top_left_point.y, p_chunk.bottom_point.y],
             color=SIGN_MARK_COLOR, linewidth=0.3) 

    plt.plot([p_chunk.top_right_point.x, p_chunk.bottom_point.x],
             [p_chunk.top_right_point.y, p_chunk.bottom_point.y],
             color=SIGN_MARK_COLOR, linewidth=0.3)

    plt.text((p_chunk.top_left_point.x + p_chunk.top_right_point.x) / 2, min(p_chunk.top_left_point.y, p_chunk.top_right_point.y),
             str(round(p_score, 2)), fontsize=5, color=SIGN_MARK_COLOR, ha='center', va='bottom', fontweight='bold')