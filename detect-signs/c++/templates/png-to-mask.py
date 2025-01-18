import cv2 as cv
import sys
import numpy as np

HUE_OFFSET = 10 
WHITE_MAX_SATURATION = 80
BLACK_MAX_VALUE = 80

RED = 256/2
WHITE = 255
BLACK = 0

SAVE_SIZE = 200

def convert_to_mask():
    if len(sys.argv) <= 1:
        print("no input img")
        return;

    img_name = sys.argv[1]
    img = cv.imread(img_name)
    width_height_ratio = img.shape[1] / img.shape[0]
    img = cv.resize(img, (SAVE_SIZE, int(SAVE_SIZE / width_height_ratio)))

    hsv_img = cv.cvtColor(img, cv.COLOR_BGR2HSV)
    mask = cv.cvtColor(img, cv.COLOR_BGR2GRAY)

    for i in range(hsv_img.shape[0]):
        for j in range(hsv_img.shape[1]):
            if hsv_img[i][j][2] < BLACK_MAX_VALUE:
                mask[i][j] = BLACK           
            elif (hsv_img[i][j][0] < HUE_OFFSET or hsv_img[i][j][0] > 179 - HUE_OFFSET) \
                and hsv_img[i][j][1] > WHITE_MAX_SATURATION:
                mask[i][j] = RED
            else:
                mask[i][j] = WHITE

    target_values = np.array([BLACK, RED, WHITE])

    cv.imwrite("crossing_mask.png", mask)

convert_to_mask()




