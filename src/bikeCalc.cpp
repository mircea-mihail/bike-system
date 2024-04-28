#include "bikeCalc.h"


TripData BikeCalc::recordDetection(int64_t p_lastWheelDetectionTime)
{        
    int64_t fullSpinDurationMicros = esp_timer_get_time() - p_lastWheelDetectionTime;
    m_data.m_currentVelocity = (WHEEL_PERIMETER_MM * SECONDS_TO_HOURS / (double)fullSpinDurationMicros);
    TripData dataToSend = m_data;

    // ignore reading, most likely fake
    if(m_data.m_currentVelocity > 100)
    {
        dataToSend.m_currentVelocity = 99;
        return dataToSend;
    }
    m_data.m_magnetDetections ++;

    int64_t distanceInKm = m_data.m_magnetDetections * WHEEL_PERIMETER_MM * SECONDS_TO_HOURS; // / MM_TO_KM * MICROS_TO_SECONDS (they cancel out)
    int64_t timeInH = (esp_timer_get_time() - m_data.m_rideStart);
    m_data.m_tripAvgVelocity = distanceInKm / timeInH;

    return dataToSend;
}


TripData BikeCalc::approximateVelocity(int64_t p_lastWheelDetectionTime)
{        
    TripData dataToSend = m_data;
    int64_t fullSpinDurationMicros = esp_timer_get_time() - p_lastWheelDetectionTime;
    uint8_t approximatedVelocity = (WHEEL_PERIMETER_MM * SECONDS_TO_HOURS / (double)fullSpinDurationMicros);
    
    approximatedVelocity = approximatedVelocity > 100 ? 99 : approximatedVelocity;
    dataToSend.m_currentVelocity = approximatedVelocity;

    int64_t distanceInKm = m_data.m_magnetDetections * WHEEL_PERIMETER_MM * SECONDS_TO_HOURS; // / MM_TO_KM * MICROS_TO_SECONDS (they cancel out)
    int64_t timeInH = (esp_timer_get_time() - m_data.m_rideStart);
    dataToSend.m_tripAvgVelocity = distanceInKm / timeInH;

    return dataToSend;
}