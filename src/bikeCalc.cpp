#include "bikeCalc.h"


int8_t BikeCalc::recordVelocity(int64_t p_lastWheelDetectionTime)
{        
    int64_t fullSpinDurationMicros = esp_timer_get_time() - p_lastWheelDetectionTime;
    m_currentVelocity = (WHEEL_PERIMETER_MM * SECONDS_TO_HOURS / (double)fullSpinDurationMicros);
    
    // ignore reading, most likely fake
    if(m_currentVelocity > 100)
    {
        return 99;
    }
    m_magnetDetections ++;

    uint32_t distanceInKm = m_magnetDetections * WHEEL_PERIMETER_MM * SECONDS_TO_HOURS;
    double timeInH = (esp_timer_get_time() - m_rideStart) / MICROS_TO_SECONDS / SECONDS_TO_HOURS;
    m_tripAvgVelocity = distanceInKm / timeInH;

    return m_currentVelocity;
}


int8_t BikeCalc::approximateVelocity(int64_t p_lastWheelDetectionTime)
{        
    int64_t fullSpinDurationMicros = esp_timer_get_time() - p_lastWheelDetectionTime;
    uint8_t approximatedVelocity = (WHEEL_PERIMETER_MM * SECONDS_TO_HOURS / (double)fullSpinDurationMicros);
    return approximatedVelocity > 100 ? 99 : approximatedVelocity;
}