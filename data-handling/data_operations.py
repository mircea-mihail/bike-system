import numpy as np
import pandas as pd
import os

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

def extractDataFromSD(sd_path):
    allFiles = os.listdir(sd_path)
    dataFiles = [file for file in allFiles if MAIN_DATA_FILE_NAME in file]

    dataFiles.sort(key = sortMethod)
    print(dataFiles)

    tripIdx = findNumberOfSavedTrips() - 1 # i do  ++ every time i find the csv header
    currentTripName = str(tripIdx) + "_" + DEFAULT_TRIP_NAME

    fileLines = 0
    inFirstFile = True
    newFileNames = []

    for dataFileName in dataFiles:
        with open(sd_path + dataFileName, 'r') as dataFile:
            for line in dataFile:
                if line == CSV_HEAD_DATA_FILE:
                    if fileLines <= MIN_LINES_IN_FILE and not(inFirstFile):
                        os.remove(TRIP_SAVE_LOCATION + currentTripName)
                    else:
                        tripIdx += 1    
                        currentTripName = str(tripIdx) + "_" + DEFAULT_TRIP_NAME
                        inFirstFile = False
                        newFileNames.append(currentTripName)
                    fileLines = 0
                
                with open(TRIP_SAVE_LOCATION + currentTripName, 'a') as newTripFile:
                    newTripFile.write(line)
                
                fileLines += 1

    if fileLines <= MIN_LINES_IN_FILE and not inFirstFile:
        os.remove(TRIP_SAVE_LOCATION + currentTripName)
        newFileNames.remove(currentTripName)

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


