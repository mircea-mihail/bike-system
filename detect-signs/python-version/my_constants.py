from dataclasses import dataclass

################################# useful structs
@dataclass
class point:
    x: float
    y: float

@dataclass 
class give_way_chunk:
    top_left_point:  point
    top_right_point: point 
    bottom_point:    point

################################# tweak-able constants
# ------------------------------- CHUNK
MIN_CHUNK_SIZE = 10
MIN_CHUNK_SCORE = 0.75
MAX_CLUMPING_VARIANCE = 5 

# ------------------------------- GIVE WAY RED BORDER   
THICK_GW_BORDER_PER_CENT = 0.356
THIN_GW_BORDER_PER_CENT = 0.72
MIN_TRIANGLE_ANGLE = 40
RED_OUTSIDE_GW_THRESHOLD = 0.4
RED_ABOVE_SIGN = (1/4)

# ------------------------------- RED COLOR DETECTION
MAX_HUE = 179
VALUE_DELIMITER = 110 
ALLOWED_RED_HUE_OFFSET = 8

BRIGHT_MIN_RED_SATURATION = 100

DARK_MIN_RED_SATURATION = 80
DARK_MIN_RED_VALUE = 50

# ------------------------------- WHITE COLOR DETECTION 
MIN_WHITE_BRIGHTNESS = 40
MAX_WHITE_SATURATION = 130

################################# rarely modified constants
IMAGE_DIR = "./output_images"

# ------------------------------- PIXLES 
RED_PX = 0
GREEN_PX = 1
BLUE_PX = 2

HUE = 0
SATURATION = 1
VALUE = 2