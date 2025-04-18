#include "bikeCalc.h"

TripData BikeCalc::recordDetection()
{        
    int64_t fullSpinDurationMicros = esp_timer_get_time() - m_lastWheelDetectionTime;
    double previousVelocity = m_data.m_currentVelocity;
    int64_t previousDetection = m_data.m_latestDetectionTime;
    double currentVelocity = (WHEEL_PERIMETER_MM * SECONDS_TO_HOURS / (double)fullSpinDurationMicros);

    //  CONVERT ACCELERATION TO MPS
    double currentAcceleration = (currentVelocity - previousVelocity) * KMPH_TO_MPS / ((esp_timer_get_time() - previousDetection) * MICROS_TO_SECONDS);
    // filter bad reading using acceleration and velocity as relevant markers
    if(currentAcceleration > MAX_ACCELERATION_MPS
        || currentVelocity > MAX_POSSIBLE_VELOCITY 
        || currentVelocity < MIN_POSSIBLE_VELOCITY)
    {
        // ignore, send previous data recording
        return m_data;
    }
    
    m_data.m_currentVelocity = currentVelocity;
    m_data.m_magnetDetections ++;
    m_data.m_previousDetectionTime = m_data.m_latestDetectionTime;
    m_data.m_latestDetectionTime = esp_timer_get_time();

    int64_t distanceInKm = m_data.m_magnetDetections * WHEEL_PERIMETER_MM * SECONDS_TO_HOURS; // / MM_TO_KM * MICROS_TO_SECONDS (they cancel out)
    int64_t timeInH = (esp_timer_get_time() - m_data.m_rideStart);
    m_data.m_tripAvgVelocity = distanceInKm / timeInH;
    m_lastWheelDetectionTime = esp_timer_get_time();
    m_data.m_previousVelocity = previousVelocity;

    return m_data;
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