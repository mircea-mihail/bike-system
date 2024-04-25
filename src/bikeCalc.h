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
    uint32_t m_magnetDetections;
    uint64_t m_rideStart;

    double m_currentVelocity;
    double m_tripAvgVelocity;

public:
    BikeCalc()
    {
        m_rideStart = esp_timer_get_time();
        m_magnetDetections = 0;
        m_currentVelocity = 0;
        m_tripAvgVelocity = 0;
    }

    int8_t recordVelocity(int64_t p_lastWheelDetectionTime);

    int8_t approximateVelocity(int64_t p_lastWheelDetectionTime);
};

#endif