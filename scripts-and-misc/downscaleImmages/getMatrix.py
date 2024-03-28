import numpy as np
from skimage import io
import glob
import cv2

NR_OF_IMAGES = 10
IMAGE_WIDTH = 77
IMAGE_HEIGHT = 125

imgList = []

for nr in range(0, NR_OF_IMAGES):
    imgList.append(cv2.imread(("%d.png" % nr), cv2.IMREAD_GRAYSCALE))

print("#include <stdbool.h>")
print("#include <Arduino.h>")

print("#define FONT_IMAGE_WIDTH 77")
print("#define FONT_IMAGE_HEIGHT 125")

print("#define NUMBER_OF_LETTERS 10")
print("#define MINUS_CHARACTER_POS 10")

print("// used progmem to store everything to flash")
print("const bool g_digitVector[NUMBER_OF_LETTERS][FONT_IMAGE_HEIGHT][FONT_IMAGE_WIDTH] PROGMEM =\n{")

for img in range(0, NR_OF_IMAGES):
    print("\t{")
    for i in range(IMAGE_HEIGHT-1, -1, -1):
        print("\t\t{", end="")
        print(0 if(imgList[img][i][0] == 0) else 1, end="")
        for j in range(1, IMAGE_WIDTH, 1):
            print(",", 0 if(imgList[img][i][j] == 0) else 1, end="")
        print("},")
    print("\t},")
print("};")