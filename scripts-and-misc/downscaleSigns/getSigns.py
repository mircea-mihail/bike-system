import numpy as np
import glob
import cv2
import os

CROSSING_WIDTH = 88
CROSSING_HEIGHT = 77

GIVE_WAY_WIDTH = 89
GIVE_WAY_HEIGHT = 77

NO_BIKES_WIDTH = 88
NO_BIKES_HEIGHT = 88

STOP_WIDTH = 88
STOP_HEIGHT = 89

NO_ENTRY_WIDTH = 88
NO_ENTRY_HEIGHT = 88

NR_OF_IMAGES = 5

imgList = []

img_path = "./signs-to-show/"
images = sorted(os.listdir(img_path))

for img_name in images:
    imgList.append(cv2.imread(os.path.join(img_path, img_name), cv2.IMREAD_GRAYSCALE))

print("#include <stdbool.h>")
print("#include <Arduino.h>")

print("#define CROSSING_WIDTH 88")
print("#define CROSSING_HEIGHT 77")

print("#define GIVE_WAY_WIDTH 89")
print("#define GIVE_WAY_HEIGHT 77")

print("#define NO_BIKES_WIDTH 88")
print("#define NO_BIKES_HEIGHT 88")

print("#define STOP_WIDTH 88")
print("#define STOP_HEIGHT 89")

print("#define NO_ENTRY_WIDTH 88")
print("#define NO_ENTRY_HEIGHT 88")

print("// used progmem to store everything to flash")
print("const bool g_crossing[CROSSING_HEIGHT][CROSSING_WIDTH] PROGMEM =\n{")
for i in range(0, CROSSING_HEIGHT):
    print("\t\t{", end="")
    print(0 if(imgList[0][i][0] == 0) else 1, end="")
    for j in range(0, CROSSING_WIDTH-1):
        print(",", 0 if(imgList[0][i][j] == 0) else 1, end="")
    print("},")
print("};")

print("const bool g_give_way[GIVE_WAY_HEIGHT][GIVE_WAY_WIDTH] PROGMEM =\n{")
for i in range(0, GIVE_WAY_HEIGHT):
    print("\t\t{", end="")
    print(0 if(imgList[1][i][0] == 0) else 1, end="")
    for j in range(0, GIVE_WAY_WIDTH-1):
        print(",", 0 if(imgList[1][i][j] == 0) else 1, end="")
    print("},")
print("};")

print("const bool g_no_bikes[NO_BIKES_HEIGHT][NO_BIKES_WIDTH] PROGMEM =\n{")
for i in range(0, NO_BIKES_HEIGHT):
    print("\t\t{", end="")
    print(0 if(imgList[2][i][0] == 0) else 1, end="")
    for j in range(0, NO_BIKES_WIDTH-1):
        print(",", 0 if(imgList[2][i][j] == 0) else 1, end="")
    print("},")
print("};")

print("const bool g_stop[STOP_HEIGHT][STOP_WIDTH] PROGMEM =\n{")
for i in range(0, STOP_HEIGHT):
    print("\t\t{", end="")
    print(0 if(imgList[3][i][0] == 0) else 1, end="")
    for j in range(0, STOP_WIDTH-1):
        print(",", 0 if(imgList[3][i][j] == 0) else 1, end="")
    print("},")
print("};")

print("const bool g_no_entry[NO_ENTRY_HEIGHT][NO_ENTRY_WIDTH] PROGMEM =\n{")
for i in range(0, NO_ENTRY_HEIGHT):
    print("\t\t{", end="")
    print(0 if(imgList[4][i][0] == 0) else 1, end="")
    for j in range(0, NO_ENTRY_WIDTH-1):
        print(",", 0 if(imgList[4][i][j] == 0) else 1, end="")
    print("},")
print("};")