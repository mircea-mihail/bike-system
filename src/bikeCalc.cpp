#include "bikeCalc.h"


TripData BikeCalc::recordDetection(int64_t p_lastWheelDetectionTime)
{        
    int64_t fullSpinDurationMicros = esp_timer_get_time() - p_lastWheelDetectionTime;
    data.m_currentVelocity = (WHEEL_PERIMETER_MM * SECONDS_TO_HOURS / (double)fullSpinDurationMicros);
    TripData dataToSend = data;

    // ignore reading, most likely fake
    if(data.m_currentVelocity > 100)
    {
        dataToSend.m_currentVelocity = 99;
        return dataToSend;
    }
    data.m_magnetDetections ++;

    uint32_t distanceInKm = data.m_magnetDetections * WHEEL_PERIMETER_MM * SECONDS_TO_HOURS;
    double timeInH = (esp_timer_get_time() - data.m_rideStart) / MICROS_TO_SECONDS / SECONDS_TO_HOURS;
    data.m_tripAvgVelocity = distanceInKm / timeInH;

    return dataToSend;
}


TripData BikeCalc::approximateVelocity(int64_t p_lastWheelDetectionTime)
{        
    TripData dataToSend = data;
    int64_t fullSpinDurationMicros = esp_timer_get_time() - p_lastWheelDetectionTime;
    uint8_t approximatedVelocity = (WHEEL_PERIMETER_MM * SECONDS_TO_HOURS / (double)fullSpinDurationMicros);
    
    approximatedVelocity = approximatedVelocity > 100 ? 99 : approximatedVelocity;
    dataToSend.m_currentVelocity = approximatedVelocity;

    return dataToSend;
}