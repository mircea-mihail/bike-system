from PIL import Image
import os
import numpy as np

RED_PX = 0
GREEN_PX = 1 
BLUE_PX = 2 

SD_PATH = "/media/mircea/930C-CA69"
DB_PATH = '/home/mircea/Documents/hobbies/proiectePersonale/bike-system/scripts-and-misc/detectSigns/my-dataset/give-way/esp-cam/new-set'

width = 320
height = 240

def show_sd():
    for img_name in sorted(os.listdir(SD_PATH)):
        file = os.path.join(SD_PATH, img_name)

        if os.path.isfile(file):
            image = np.zeros((height, width, 3), dtype=np.uint8)

            # Open the file in binary mode
            with open(file, 'rb') as file:
                for i in range(0, height):
                    for j in range(0, width):
                        # Read one byte at a time
                        rgb = int.from_bytes(file.read(2), byteorder="big")

                        red = rgb >> 11
                        green = (rgb & 0b0000011111100000) >> 5
                        blue = rgb & 0b0000000000011111

                        max_red = 32
                        max_green = 64
                        max_blue = 32
                        max_pixel = 255

                        red = max_pixel * (red / max_red)
                        green = max_pixel * (green / max_green)
                        blue = max_pixel * (blue / max_blue)
                    
                        image[i][j][RED_PX] = red
                        image[i][j][GREEN_PX] = green 
                        image[i][j][BLUE_PX] = blue
                    
            # Convert to PIL Image
            image_pil = Image.fromarray(image)

            # Save as PNG
            image_pil.save(os.path.join(DB_PATH, os.path.splitext(img_name)[0] + ".jpeg"), 'JPEG', quality=100)
            # plt.imshow(image)
            # plt.show()
    
show_sd()
