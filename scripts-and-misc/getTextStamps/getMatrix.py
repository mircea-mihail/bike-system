import numpy as np
from skimage import io
import glob
import cv2

NR_OF_IMAGES = 1
IMAGE_WIDTH = 50
IMAGE_HEIGHT = 50

imgList = []

for nr in range(0, NR_OF_IMAGES):
    imgList.append(cv2.imread(("stamp%d.png" % nr), cv2.IMREAD_GRAYSCALE))

print("#include <stdbool.h>")
print("#include <Arduino.h>")
print()
print("#define STAMP_WIDTH 50")
print("#define STAMP_HEIGHT 50")
print()
print("#define NUMBER_OF_STAMPS 1")
print("#define MINUS_CHARACTER_POS 10")
print()
print("// stamp indexes:")
print("#define KMPH_STAMP 0")
print()

print("// used progmem to store everything to flash")
print("const bool g_stampVector[NUMBER_OF_STAMPS][STAMP_HEIGHT][STAMP_WIDTH] PROGMEM =\n{")

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