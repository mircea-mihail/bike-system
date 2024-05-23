#include "bikeCalc.h"

/// @brief records the latest hall sensor reading and updates all member variables dependent on it
/// @param p_lastWheelDetectionTime the time that the reading took pl
/// @return 
TripData BikeCalc::recordDetection()
{        
    int64_t fullSpinDurationMicros = esp_timer_get_time() - m_lastWheelDetectionTime;
    double previousVelocity = m_data.m_currentVelocity;
    m_data.m_currentVelocity = (WHEEL_PERIMETER_MM * SECONDS_TO_HOURS / (double)fullSpinDurationMicros);
    TripData dataToSend = m_data;

    // ignore reading, most likely fake
    if(m_data.m_currentVelocity - m_data.m_previousVelocity > MAX_DELTA_VELOCITY_KMPH 
        || m_data.m_currentVelocity > MAX_POSSIBLE_VELOCITY 
        || m_data.m_currentVelocity < MIN_POSSIBLE_VELOCITY)
    {
        // ignore, send previous data recording
        return dataToSend;
    }
    m_data.m_magnetDetections ++;

    int64_t distanceInKm = m_data.m_magnetDetections * WHEEL_PERIMETER_MM * SECONDS_TO_HOURS; // / MM_TO_KM * MICROS_TO_SECONDS (they cancel out)
    int64_t timeInH = (esp_timer_get_time() - m_data.m_rideStart);
    m_data.m_tripAvgVelocity = distanceInKm / timeInH;
    m_lastWheelDetectionTime = esp_timer_get_time();
    m_data.m_previousVelocity = previousVelocity;


    return dataToSend;
}


TripData BikeCalc::approximateVelocity()
{        
    if(m_data.m_magnetDetections == 0)
    {
        return m_data;
    }

    TripData dataToSend = m_data;
    int64_t fullSpinDurationMicros = esp_timer_get_time() - m_lastWheelDetectionTime;
    uint8_t approximatedVelocity = (WHEEL_PERIMETER_MM * SECONDS_TO_HOURS / (double)fullSpinDurationMicros);
    
    approximatedVelocity = approximatedVelocity > 100 ? 99 : approximatedVelocity;
    dataToSend.m_currentVelocity = approximatedVelocity;

    int64_t distanceInKm = m_data.m_magnetDetections * WHEEL_PERIMETER_MM * SECONDS_TO_HOURS; // / MM_TO_KM * MICROS_TO_SECONDS (they cancel out)
    int64_t timeInH = (esp_timer_get_time() - m_data.m_rideStart);
    dataToSend.m_tripAvgVelocity = distanceInKm / timeInH;

    return dataToSend;
}