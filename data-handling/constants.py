import numpy as np

# -------------------------- FILE AND PATH
BIKE_DATA_PATH = "/media/mircea/CEF1-30A0/data/"
TRIP_SAVE_LOCATION = "/home/mircea/.bike-sys/"
 
MAIN_DATA_FILE_NAME = "_speed_acc.txt"
CSV_HEAD_DATA_FILE = "detection time micros\n" 
DEFAULT_TRIP_NAME = "trip.txt"

# -------------------------- CONVERSIONS
MM_TO_KM = 1_000_000
MM_TO_M = 1000
KMPH_TO_MPS = 1000 / 3_600
MICROS_TO_HOURS = 1000 * 1000 * 3_600
MMPUS_TO_KMPH = 3_600

# -------------------------- MEASUREMENTS
BIKE_WHEEL_DIAMETER_MM = 700
BIKE_WHEEL_PERIMETER_MM = BIKE_WHEEL_DIAMETER_MM * np.pi

# -------------------------- PRINTING 
DISTANCES_TO_PRINT = 10

# -------------------------- FILE FILTERING
# at least 10 meters in a trip
MIN_KM_IN_TRIP = 0.01
MIN_LINES_IN_FILE = MIN_KM_IN_TRIP * MM_TO_KM / BIKE_WHEEL_DIAMETER_MM
