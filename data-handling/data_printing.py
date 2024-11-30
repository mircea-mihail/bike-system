from constants import *

# print general information about tripData
def printTripData(tripData):
    print("fastest speed:\t" + str(tripData["maxSpeed"]) + " KM/H")
    print("average speed:\t" + str(round(tripData["averageSpeed"], 2)) + " KM/H")
    print()
    print("longest trip:\t" + str(round(tripData["longestTripLength"], 2)) + " KM")
    print("total distance:\t" + str(round(tripData["totalDistance"], 2)) + " KM")
    print("avg distance:\t" + str(round(tripData["averageTripDistance"], 2)) + " KM")
    print("latest dists:\t", end="")
    
    distancesToPrint = DISTANCES_TO_PRINT
    if len(tripData["distanceList"]) < DISTANCES_TO_PRINT:
        distancesToPrint = len(tripData["distanceList"])

    for idx in range(len(tripData["distanceList"]) - 1, len(tripData["distanceList"]) - distancesToPrint - 1, -1):
        print(round(tripData["distanceList"][idx], 2), end=", ")


