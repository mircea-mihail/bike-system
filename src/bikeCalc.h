#ifndef BIKE_CALC_H
#define BIKE_CACL_H

#include <Arduino.h>
#include "generalUtility.h"

// real-world constants
#define WHEEL_DIAMETER_MM 700UL
#define WHEEL_PERIMETER_MM (WHEEL_DIAMETER_MM * PI)

#define MAX_DELTA_VELOCITY_KMPH 30
#define MAX_POSSIBLE_VELOCITY 99
#define MIN_POSSIBLE_VELOCITY 0

class BikeCalc
{
private:
    TripData m_data;
    int64_t m_lastWheelDetectionTime;

public:
    BikeCalc()
    {
        m_data.m_rideStart = esp_timer_get_time();
        m_data.m_magnetDetections = 0;
        m_data.m_currentVelocity = 0;
        m_data.m_tripAvgVelocity = 0;
        m_data.m_previousVelocity = 0;
        m_lastWheelDetectionTime = 0;
    }

    TripData recordDetection();

    TripData approximateVelocity();
};

#endif