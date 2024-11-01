import os
import cv2 as cv
import time

from detect_give_way import *

def get_photo():
    # write take picture algorithm
    IMG_DIR = '/home/mircea/Documents/hobbies/proiectePersonale/bike-system/detect-signs/my-dataset/give-way/phone-cam/easy_thick_sign_set/the_dataset/'
    return cv.imread(IMG_DIR + sorted(os.listdir(IMG_DIR))[2])

total_start = time.time()
start = time.time()
img = get_photo()
print("get from dir took", time.time() - start)
start = time.time()

img = cv.medianBlur(img, 5)
print("median blur took", time.time() - start)
start = time.time()

img = cv.resize(img, (240, 320))
print("resize took", time.time() - start)
start = time.time()

detection_score, gw_chunk = detect_gw(img) 
duration = time.time() - start
print("my-algo took", duration)
print("detection accuracy:", detection_score, "in", time.time() - total_start)

cv.line(img, (gw_chunk.bottom_point.x, gw_chunk.bottom_point.y), (gw_chunk.top_left_point.x, gw_chunk.top_left_point.y), (255, 0, 255), 1)
cv.line(img, (gw_chunk.top_right_point.x, gw_chunk.top_right_point.y), (gw_chunk.top_left_point.x, gw_chunk.top_left_point.y), (255, 0, 255), 1)
cv.line(img, (gw_chunk.bottom_point.x, gw_chunk.bottom_point.y), (gw_chunk.top_right_point.x, gw_chunk.top_right_point.y), (255, 0, 255), 1)
cv.imshow("cedeaza", img)
cv.waitKey(0)
cv.destroyAllWindows()

