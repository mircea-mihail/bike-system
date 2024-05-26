#ifndef GENERAL_UTILITY_H
#define GENERAL_UTILITY_H

// convenient conversions
#define MM_TO_KM 1000000UL
#define MM_TO_M 1000UL
#define M_TO_KM 1000UL
#define MILLIS_TO_MICROS 1000UL
#define MICROS_TO_SECONDS 1000000UL
#define SECONDS_TO_HOURS 3600UL
#define MS_TO_SECONDS 1000

// hardware debounce timings
#define SENSOR_DEBOUNCE_PERIOD_MS 10UL
#define BUTTON_CONSTANT_STATE_MS 70UL

/////////////////////////////// pins
// input pins
#define HALL_SENSOR_PIN GPIO_NUM_39
#define CHANGE_SUBMENU_PIN GPIO_NUM_36
#define CHANGE_MENU_PIN GPIO_NUM_34

// SPI pins
#define SD_CHIP_SELECT GPIO_NUM_5
#define DISPLAY_CHIP_SELECT GPIO_NUM_33     // SPI chip selection, low active

#define DATA_OUT GPIO_NUM_23        // SPI MOSI pin, used designed pin for this on esp
#define CLK GPIO_NUM_18             // SPI SCK pin, used designed pin for this on esp
#define DATA_COMMAND GPIO_NUM_12    // Data/Command selection (high for data, low for command) -> am pus pinul de MISO

// pins for e-paper display
#define RST GPIO_NUM_27             // External reset, low active
#define BUSY GPIO_NUM_35            // Busy status output, high active

// data packet to send to menu
struct TripData
{
    uint32_t m_magnetDetections;
    uint64_t m_rideStart;

    double m_currentVelocity;
    double m_previousVelocity;
    double m_tripAvgVelocity;   

    TripData()
    {
        m_magnetDetections = 0; 
        m_rideStart = 0;
        m_currentVelocity = 0;
        m_previousVelocity = 0;
        m_tripAvgVelocity = 0;
    }

    bool operator== (const TripData p_rhs)
    {
        if(this->m_magnetDetections == p_rhs.m_magnetDetections
            && this->m_rideStart == p_rhs.m_rideStart
            && this->m_currentVelocity == p_rhs.m_currentVelocity
            && this->m_previousVelocity == p_rhs.m_previousVelocity
            && this->m_tripAvgVelocity == p_rhs.m_tripAvgVelocity
        )
        {
            return true;
        }
        return false;
    }

    bool operator!= (const TripData p_rhs)
    {
        if(this->m_magnetDetections == p_rhs.m_magnetDetections
            && this->m_rideStart == p_rhs.m_rideStart
            && this->m_currentVelocity == p_rhs.m_currentVelocity
            && this->m_previousVelocity == p_rhs.m_previousVelocity
            && this->m_tripAvgVelocity == p_rhs.m_tripAvgVelocity
        )
        {
            return false;
        }
        return true;
    }

    TripData & operator= (const TripData& p_rhs){
        if(this != &p_rhs)
        {
            this->m_magnetDetections = p_rhs.m_magnetDetections;
            this->m_rideStart = p_rhs.m_rideStart;
            this->m_currentVelocity = p_rhs.m_currentVelocity;
            this->m_previousVelocity = p_rhs.m_previousVelocity;
            this->m_tripAvgVelocity = p_rhs.m_tripAvgVelocity;
        }
        return *this;
    }
};

#endif