# %%
import os

import numpy as np
import pandas as pd

from matplotlib import pyplot as plt
from matplotlib.pyplot import figure
from collections import Counter

from constants import *
from data_operations import *
from data_printing import *



# %%
back_up_data(BIKE_DATA_PATH)

# %%
#   * split the collected data into trips (data is stored in equally sized chunks)
#   * delete everything on the sd card to make room for new data
#   * print facts about the new trips

extractDataFromSD(BIKE_DATA_PATH)

# %%
# run this to plot:
#   * the latest two trips speed evolution (big picture)
#   * 4 other trips speed evolution before them
FILE_DPI = 130

FIGURE_COLS = 1
FIGURE_ROWS = 2

PAST_TRIPS_TO_SHOW = 4

TRIP_TO_SHOW = 2
OUTPUT_DIR = "./graphs"

if not(Path(OUTPUT_DIR).exists()):
    os.makedirs(OUTPUT_DIR)


tripFiles = os.listdir(TRIP_SAVE_LOCATION)
tripFiles = [file for file in tripFiles if file.endswith(('.csv', '.txt'))]
tripFiles.sort(key = fileSortMethod, reverse=True)

# big plot of the past two trips followed by small plots of 5 previous trips 
figure(figsize=(15, 7), dpi=FILE_DPI)
tripCsv = pd.read_csv(TRIP_SAVE_LOCATION + tripFiles[0])
velocityList = getVelocityList(tripCsv['detection time micros'])
velocityListLen = len(velocityList)


distanceList = np.linspace(0, velocityListLen, velocityListLen)
distanceList = [distMarker * (BIKE_WHEEL_PERIMETER_MM / MM_TO_KM) for distMarker in distanceList]

plt.subplot(FIGURE_ROWS, FIGURE_COLS, 1)
plt.plot(distanceList, velocityList, color="purple")
plt.xlabel("distance (KM)")
plt.ylabel("velocity (KM/H)")
plt.title("last trip")
plt.axhline(0, color="black")

tripCsv = pd.read_csv(TRIP_SAVE_LOCATION + tripFiles[1])
velocityList = getVelocityList(tripCsv['detection time micros'])
velocityListLen = len(velocityList)
distanceList = np.linspace(0, velocityListLen, velocityListLen)
distanceList = [distMarker * (BIKE_WHEEL_PERIMETER_MM / MM_TO_KM) for distMarker in distanceList]

plt.subplot(FIGURE_ROWS, FIGURE_COLS, 2)
plt.plot(distanceList, velocityList, color="purple")
plt.xlabel("distance (KM)")
plt.ylabel("velocity (KM/H)")
plt.title("the run before")
plt.axhline(0, color="black")

plt.subplots_adjust(hspace=0.4)

plt.savefig(os.path.join(OUTPUT_DIR, "0-past-2-trips.png"), bbox_inches='tight')
plt.close()  

################# smaller plots
SUB_FIGURE_COLS = 2
SUB_FIGURE_ROWS = 2

figure(figsize=(15, 4), dpi=FILE_DPI)

trips_to_show = len(tripFiles) - 2
for i in range(0, PAST_TRIPS_TO_SHOW):
    tripCsv = pd.read_csv(TRIP_SAVE_LOCATION + tripFiles[i + 2])
    velocityList = getVelocityList(tripCsv['detection time micros'])
    velocityListLen = len(velocityList)
    
    distanceList = np.linspace(0, velocityListLen, velocityListLen)
    distanceList = [distMarker * (BIKE_WHEEL_PERIMETER_MM / MM_TO_KM) for distMarker in distanceList]

    plt.subplot(SUB_FIGURE_ROWS, SUB_FIGURE_COLS, i + 1)
    plt.plot(distanceList, velocityList, color="purple")
    plt.xlabel("distance (KM)")
    plt.ylabel("velocity (KM/H)")
    plt.title(str(i + 2) + " trips before")
    plt.axhline(0, color="black")


plt.subplots_adjust(hspace=0.6)

plt.savefig(os.path.join(OUTPUT_DIR, "0-past-4-trips.png"), bbox_inches='tight')
plt.close()  

# %%
# show stats on all trips

# show most commons speeds (top 5?)
# do a pie chart of like 5 bins

 
print("\tAll time stats:")
tripFiles = os.listdir(TRIP_SAVE_LOCATION)
tripFiles = [file for file in tripFiles if file.endswith(('.csv', '.txt'))]
tripFiles.sort(key = fileSortMethod, reverse=True)

# tripFiles = os.listdir(TRIP_SAVE_LOCATION)

printTripData(getTripData(tripFiles))
# print("\n\n\tLast week stats")

velocityBins = [0, 8, 17, 23, 30, 80]
binsLables = ["on foot\n0-8 KM/H", "slow pace\n8-17 KM/H", "normal pace\n17-23 KM/H", "fast pace\n23-30 KM/H", "very fast pace\n30+ KM/H"]

averageHistogram = np.zeros(len(binsLables))
speedCounter = Counter()
       
for tripFileName in tripFiles:
    tripCsv = pd.read_csv(TRIP_SAVE_LOCATION + tripFileName)
    velocityList = getVelocityList(tripCsv['detection time micros'])
    
    histogram, trash = np.histogram(velocityList,velocityBins)
    histogram = histogram/len(velocityList)
    averageHistogram += histogram
    speedCounter.update([round(speed, 0) for speed in velocityList])

figure(figsize=(9,5), dpi=FILE_DPI)
plt.subplot(1, 2, 1)
# patch for bigger problem
mostCommonSpeeds = np.array([speed for speed in speedCounter.most_common(len(speedCounter)) if speed[0] < 50])

plt.bar(mostCommonSpeeds[:, 0], mostCommonSpeeds[:, 1] * BIKE_WHEEL_PERIMETER_MM / MM_TO_KM, width=0.8, color="purple")
plt.title("velocity distribution accross distance")
plt.xlabel("velocity (KM/H)")
plt.ylabel("KM travelled (KM)")


plt.subplot(1, 2, 2)
mostCommonSpeeds = np.array(speedCounter.most_common(10))
plt.bar(mostCommonSpeeds[:, 0], mostCommonSpeeds[:, 1] * BIKE_WHEEL_PERIMETER_MM / MM_TO_KM, width=0.8, color="purple")
plt.title("top 10 most common velocities")
plt.xlabel("velocity (KM/H)")
plt.ylabel("KM travelled (KM)")
plt.savefig(os.path.join(OUTPUT_DIR, "1-all-time-granular-velocity-distribution.png"), bbox_inches='tight')
plt.close() 

averageHistogram = averageHistogram / len(tripFiles)

figure(figsize=(6, 6), dpi=FILE_DPI)
plt.title("Distance traveled at each speed")
patches, labels, autotexts = plt.pie(averageHistogram, labels=binsLables, autopct='%1.2f%%')

# labels[0]._y += 0.1
# labels[4]._y -= 0.1

plt.tight_layout()
plt.savefig(os.path.join(OUTPUT_DIR, "1-all-time-velocity-pie-chart.png"), bbox_inches='tight')
plt.close()  


# %%
# run this to see corelations between trips
tripFiles = os.listdir(TRIP_SAVE_LOCATION)
tripFiles = [file for file in tripFiles if file.endswith(('.csv', '.txt'))]
print(len(tripFiles))
tripFiles.sort(key = fileSortMethod)

averageTripSpeed = []
tripDistance = []

for tripFileName in tripFiles:
    tripCsv = pd.read_csv(TRIP_SAVE_LOCATION + tripFileName)
    velocityList = getVelocityList(tripCsv['detection time micros'])
    timeList = tripCsv['detection time micros']
    timeListLen = len(timeList)

    tripDistance.append(timeListLen)
    averageTripSpeed.append(timeListLen * BIKE_WHEEL_PERIMETER_MM / timeList[timeListLen - 1] * MMPUS_TO_KMPH)

tripDistance = [dist * BIKE_WHEEL_PERIMETER_MM / MM_TO_KM for dist in tripDistance]

figure(dpi=FILE_DPI)
plt.scatter(tripDistance, averageTripSpeed, color="purple")
plt.ylabel("average velocity (KM/H)")
plt.xlabel("distance travelled (KM)")
plt.title("trip corelations")

plt.savefig(os.path.join(OUTPUT_DIR, "2-all-time-data-relations.png"), bbox_inches='tight')
plt.close()  

# %%
# run this to plot:
#   interesting things about the latest trip

TRIP_TO_SHOW = 0

tripFiles = os.listdir(TRIP_SAVE_LOCATION)
tripFiles = [file for file in tripFiles if file.endswith(('.csv', '.txt'))]
tripFiles.sort(key = fileSortMethod, reverse=True)

tripCsv = pd.read_csv(TRIP_SAVE_LOCATION + tripFiles[TRIP_TO_SHOW])
velocityList = getVelocityList(tripCsv['detection time micros'])
velocityListLen = len(velocityList)

deltaVelList = np.zeros(velocityListLen)
for i in range(1, velocityListLen):
    deltaVelList[i] = velocityList[i] - velocityList[i-1]

accList = [accVal * KMPH_TO_MPS/(2 * BIKE_WHEEL_PERIMETER_MM / MM_TO_M) for accVal in deltaVelList]
# accList = [accVal / 36 * 1000  / 36 / 2 / 7 / 2 / np.pi for accVal in accList]

accList[0] = 0 
for idx in range(1, len(accList)):
    accList[idx] *= (velocityList[idx] + velocityList[idx - 1]) * KMPH_TO_MPS
    
    if(accList[idx] > 2):
        print(velocityList[idx])
distanceList = np.linspace(0, len(accList), len(accList))
distanceList = [distMarker * (BIKE_WHEEL_PERIMETER_MM / MM_TO_KM) for distMarker in distanceList]


FIGURE_COLS = 1
FIGURE_ROWS = 2

# plt.subplot(FIGURE_ROWS, FIGURE_COLS, 1)
figure(figsize=(15, 7), dpi=FILE_DPI)
plt.subplot(FIGURE_ROWS, FIGURE_COLS, 1)
plt.plot(distanceList, velocityList, color="purple")
plt.xlabel("distance (KM)")
plt.ylabel("velocity (KM/H)")
plt.axhline(0, color="black")
plt.title("evolution of velocity in a trip")

plt.subplot(FIGURE_ROWS, FIGURE_COLS, 2)
plt.plot(distanceList, accList, color="purple")
plt.ylabel("acceleration (m/s^2))")
plt.xlabel("distance (KM)")
plt.title("evolution of acceleration in a trip")
plt.axhline(0, color="black")
# plt.axhline(5, color="red")
# plt.axhline(2.5, color="orange")

plt.subplots_adjust(hspace=0.4)
plt.savefig(os.path.join(OUTPUT_DIR, "3-in-depth-distance-related.png"), bbox_inches='tight')
plt.close()  

figure(figsize=(15, 7), dpi=FILE_DPI)
plt.subplot(FIGURE_ROWS, FIGURE_COLS, 1)
plt.scatter(velocityList, accList, s=5, color="purple")
plt.xlabel("velocity (KM/H)")
plt.ylabel("acceleration (m/s^2))")
plt.title("acceleration and velocity")
accAverage = sum(accList)/len(accList)
print("Total acceleration:", accAverage)
plt.axhline(y=accAverage, color='black', linewidth=1)

plt.subplot(FIGURE_ROWS, FIGURE_COLS, 2)
plt.scatter(velocityList, deltaVelList, s=5, color="purple")
plt.xlabel("velocity")
plt.ylabel("delta V (KM/H)")
plt.title("change in speed and velocity (KM/H)")
plt.axhline(y=0, color='black', linewidth=1)

plt.subplots_adjust(hspace=0.4)
plt.savefig(os.path.join(OUTPUT_DIR, "3-in-depth-acceleration-related.png"), bbox_inches='tight')
plt.close()  

# uncomment to plot acceleration and velocity graph again, but wider
# make every point a slightly different hue
figure(figsize=(15, 7), dpi=200)
plt.scatter(velocityList, accList, s=0.5, color="purple")
plt.xlabel("velocity (KM/H)")
plt.ylabel("acceleration (m/s^2))")
plt.title("the most interesting plot")
accAverage = sum(accList)/len(accList)
print(accAverage)
plt.axhline(y=accAverage, color='black', linewidth=1)
plt.savefig(os.path.join(OUTPUT_DIR, "3-high-rez-acc-velocity.png"), bbox_inches='tight')
plt.close()  


