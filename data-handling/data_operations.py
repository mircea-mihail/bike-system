import numpy as np
import pandas as pd
import os
import shutil
from pathlib import Path

from constants import *
from general_purpose import *
from data_printing import *



# a couple of functions to make stat printing easier

# from a list of moments of detections return a list of velocities at every point of detection
def getVelocityList(momentMicrosList):
    tripLength = len(momentMicrosList)

    velocityList = np.zeros(tripLength)

    # velocity is distance over speed
    if momentMicrosList[0] * MMPUS_TO_KMPH != 0:
        velocityList[0] = BIKE_WHEEL_PERIMETER_MM / momentMicrosList[0] * MMPUS_TO_KMPH
    else:
        velocityList[0] = 0

    for i in range(1, tripLength):
        velocityList[i] = BIKE_WHEEL_PERIMETER_MM / (momentMicrosList[i] - momentMicrosList[i-1]) * MMPUS_TO_KMPH

    return velocityList

# build a trip data dictionary with facts about trips in tripFiles 
def getTripData(tripFiles):
    tripDataNames = ["maxSpeed", "averageSpeed", "longestTripLength", "totalDistance", "averageTripDistance", "distanceList"]
    tripData = {}
    totalTime = 0

    if(len(tripFiles) == 0):
        for dataName in tripDataNames:
            if(dataName == "distanceList"):
                tripData[dataName] = [0]
            else:
                tripData[dataName] = 0
        return tripData
    
    for dataName in tripDataNames:
        if dataName != "distanceList":
            tripData[dataName] = 0
        else:
            tripData["distanceList"] = []

    for tripFileName in tripFiles:
        tripCsv = pd.read_csv(TRIP_SAVE_LOCATION + tripFileName)
        momentMicrosList = tripCsv['detection time micros']
        
        tripLength = len(momentMicrosList)
        tripData["distanceList"].append(tripLength)
        tripData["totalDistance"] += tripLength
        
        velocityList = getVelocityList(momentMicrosList)
        tripMaxSpeed = max(velocityList)
        totalTime += momentMicrosList[len(momentMicrosList) - 1]
        # tripData["averageSpeed"] += sum(velocityList)/len(velocityList)

        if tripMaxSpeed > tripData["maxSpeed"]:
            tripData["maxSpeed"] = tripMaxSpeed
        if tripLength > tripData["longestTripLength"]:
            tripData["longestTripLength"] = tripLength
    
    # V = D / T
    tripData["longestTripLength"] = tripData["longestTripLength"] * BIKE_WHEEL_PERIMETER_MM / MM_TO_KM
    tripData["totalDistance"] = tripData["totalDistance"] * BIKE_WHEEL_PERIMETER_MM / MM_TO_KM
    tripData["averageSpeed"] = tripData["totalDistance"] / (totalTime / MICROS_TO_HOURS)
    tripData["averageTripDistance"] = tripData["totalDistance"] / len(tripFiles)

    for distanceIdx in range(0, len(tripData["distanceList"])):
        tripData["distanceList"][distanceIdx] = tripData["distanceList"][distanceIdx] * BIKE_WHEEL_PERIMETER_MM / MM_TO_KM

    return tripData


def findNumberOfSavedTrips():
    tripFiles = os.listdir(TRIP_SAVE_LOCATION)
    return len(tripFiles)

def back_up_data(sd_path):
    if not(Path(BACKUP_SAVE_LOCATION).exists()):
        os.makedirs(BACKUP_SAVE_LOCATION)

    backup_dirs = os.listdir(BACKUP_SAVE_LOCATION)
    int_backup_dirs = [int(dir) for dir in backup_dirs]
    if len(int_backup_dirs) == 0:
        current_backup_dir = "0"
    else:
        current_backup_dir = str(max(int_backup_dirs) + 1)

    os.makedirs(os.path.join(BACKUP_SAVE_LOCATION, current_backup_dir))

    allFiles = os.listdir(sd_path)
    dataFiles = [file for file in allFiles if MAIN_DATA_FILE_NAME in file]

    for dataFileName in dataFiles:
        src_file_path = os.path.join(sd_path, dataFileName)
        dst_file_path = os.path.join(os.path.join(BACKUP_SAVE_LOCATION, current_backup_dir), dataFileName)
        shutil.copy2(src_file_path, dst_file_path)

def extractDataFromSD(sd_path):
    allFiles = os.listdir(sd_path)
    dataFiles = [file for file in allFiles if MAIN_DATA_FILE_NAME in file]

    dataFiles.sort(key = fileSortMethod)
    print(dataFiles)

    tripIdx = findNumberOfSavedTrips() - 1 # i do  ++ every time i find the csv header
    currentTripName = str(tripIdx) + "_" + DEFAULT_TRIP_NAME

    fileLines = 0
    inFirstFile = True
    newFileNames = []

    current_max_speed = 0
    current_max_acceleration = 0
    prev_line = ""
    prev_speed = 0

    noisy_trips = 0
    short_trips = 0

    for dataFileName in dataFiles:
        with open(sd_path + dataFileName, 'r') as dataFile:
            for line in dataFile:
                if line == CSV_HEAD_DATA_FILE:
                    if (fileLines <= MIN_LINES_IN_FILE and not(inFirstFile))\
                        or current_max_speed > MAX_POSSIBLE_SPEED_KMPH \
                        or current_max_acceleration > MAX_POSSIBLE_ACCELERATION_MPSS:

                        if current_max_speed > MAX_POSSIBLE_SPEED_KMPH or current_max_acceleration > MAX_POSSIBLE_ACCELERATION_MPSS:
                            print("filtered file for ", current_max_acceleration, "m/ss max acceleration and ", current_max_speed, "kmph max speed")
                            noisy_trips += 1 
                        else:
                            print("filtered file for being only ", fileLines * BIKE_WHEEL_DIAMETER_MM/MM_TO_KM, "km long")
                            short_trips += 1

                        os.remove(TRIP_SAVE_LOCATION + currentTripName)
                    else:
                        tripIdx += 1    
                        currentTripName = str(tripIdx) + "_" + DEFAULT_TRIP_NAME
                        inFirstFile = False
                        newFileNames.append(currentTripName)
                    fileLines = 0

                    current_max_speed = 0
                    current_max_acceleration = 0
                    prev_line = ""
                    prev_speed = 0
                # set max acceleration and speed to filter out noisy trips
                else:
                    if prev_line != "":
                        current_speed = BIKE_WHEEL_PERIMETER_MM / (int(line) - int(prev_line)) * MMPUS_TO_KMPH
                        if current_speed > current_max_speed:
                            current_max_speed = current_speed
                        current_acceleration = (current_speed - prev_speed) * KMPH_TO_MPS/(2 * BIKE_WHEEL_PERIMETER_MM / MM_TO_M)
                        if current_acceleration > current_max_acceleration:
                            current_max_acceleration = current_acceleration
                        prev_speed = current_speed

                    prev_line = line
                    
                
                with open(TRIP_SAVE_LOCATION + currentTripName, 'a') as newTripFile:

                    newTripFile.write(line)
                
                fileLines += 1

    if fileLines <= MIN_LINES_IN_FILE and not inFirstFile:
        os.remove(TRIP_SAVE_LOCATION + currentTripName)
        newFileNames.remove(currentTripName)

    print("\nFILTERED ", noisy_trips, " NOISY TRIPS\n\n")

    print("created:")
    print(newFileNames)

    if len(dataFiles) > 0:          
        print()
        print("facts about the new data:")
        printTripData(getTripData(newFileNames))

    # run to delete SD contents

    if len(dataFiles) > 0:
        for dataFileName in dataFiles:
            os.remove(BIKE_DATA_PATH + dataFileName)


