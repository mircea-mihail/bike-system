#ifndef BIKE_CALC_H
#define BIKE_CACL_H

#include <Arduino.h>
#include "generalUtility.h"

// real-world constants
#define WHEEL_DIAMETER_MM 700UL
#define WHEEL_PERIMETER_MM (WHEEL_DIAMETER_MM * PI)

class BikeCalc
{
private:
    TripData m_data;
    
public:
    BikeCalc()
    {
        m_data.m_rideStart = esp_timer_get_time();
        m_data.m_magnetDetections = 0;
        m_data.m_currentVelocity = 0;
        m_data.m_tripAvgVelocity = 0;
    }

    TripData recordDetection(int64_t p_lastWheelDetectionTime);

    TripData approximateVelocity(int64_t p_lastWheelDetectionTime);
};

#endif