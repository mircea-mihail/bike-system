#ifndef BIKE_CALC_H
#define BIKE_CACL_H

#include <Arduino.h>
#include "generalUtility.h"

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
        m_data.m_latestDetectionTime = 0;
        m_data.m_tripAvgVelocity = 0;
        m_data.m_previousVelocity = 0;
        m_lastWheelDetectionTime = 0;
    }

    /// @brief records the latest hall sensor reading and updates all member variables dependent on it
    ///     filters readings that are for sure wrong
    /// @return returns computed trip data 
    TripData recordDetection();

    /// @brief approximates speed as if it just had a sensor reading and returns the computed
    ///     it also filters readings that are for sure wrong
    /// @return returns computed trip data 
    TripData approximateVelocity();
};

#endif