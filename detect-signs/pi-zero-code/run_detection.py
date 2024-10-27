from picamera2 import Picamera2
import os
import cv2 as cv
import time

from detect_give_way import *
# Initialize the Picamera2 object
picam2 = Picamera2()

camera_config = picam2.create_still_configuration(
    main={"size": (INITIAL_IMG_WIDTH, INITIAL_IMG_HEIGHT)}
    )

picam2.configure(camera_config)

# Start the camera
picam2.start()
time.sleep(2)  # Allow the camera to warm up

def get_photo():
    # Capture an image into a variable
    img = picam2.capture_array()
    img = cv.cvtColor(img, cv.COLOR_RGB2BGR)
    return img

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

detection_score= detect_gw(img) 
duration = time.time() - start
print("my-algo took", duration)
print("detection accuracy:", detection_score, "in", time.time() - total_start)

# cv.line(img, (gw_chunk.bottom_point.x, gw_chunk.bottom_point.y), (gw_chunk.top_left_point.x, gw_chunk.top_left_point.y), (255, 0, 255), 1)
# cv.line(img, (gw_chunk.top_right_point.x, gw_chunk.top_right_point.y), (gw_chunk.top_left_point.x, gw_chunk.top_left_point.y), (255, 0, 255), 1)
# cv.line(img, (gw_chunk.bottom_point.x, gw_chunk.bottom_point.y), (gw_chunk.top_right_point.x, gw_chunk.top_right_point.y), (255, 0, 255), 1)
# cv.imshow("cedeaza", img)
# cv.waitKey(0)
# cv.destroyAllWindows()

# Stop the camera
picam2.stop()


