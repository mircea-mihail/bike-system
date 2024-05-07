#ifndef GENERAL_UTILITY_H
#define GENERAL_UTILITY_H


// convenient conversions
#define MM_TO_KM 1000000UL
#define MILLIS_TO_MICROS 1000UL
#define MICROS_TO_SECONDS 1000000UL
#define SECONDS_TO_HOURS 3600UL

// hardware debounce timings
#define SENSOR_DEBOUNCE_PERIOD_MS 10UL
#define BUTTON_DEBOUNCE_PERIOD_MS 10UL

/////////////////////////////// pins
// input pins
#define HALL_SENSOR_PIN GPIO_NUM_39
#define CHANGE_SUBMENU_PIN GPIO_NUM_36

#define CHANGE_MENU_PIN GPIO_NUM_34

// data packet to send to menu
struct TripData
{
    uint32_t m_magnetDetections;
    uint64_t m_rideStart;

    double m_currentVelocity;
    double m_tripAvgVelocity;   
};

#endif