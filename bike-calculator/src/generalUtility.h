#ifndef GENERAL_UTILITY_H
#define GENERAL_UTILITY_H

// real world defines
#define BIKE_MASS_KG 10
#define RIDER_MASS_KG 60
#define BAG_MASS_KG 2
#define TOTAL_MASS_KG (BIKE_MASS_KG + RIDER_MASS_KG + BAG_MASS_KG) 

#define WHEEL_DIAMETER_MM 700UL
#define WHEEL_PERIMETER_MM (WHEEL_DIAMETER_MM * PI)

#define MAX_ACCELERATION_MPS 5 
#define MAX_POSSIBLE_VELOCITY 99
#define MIN_POSSIBLE_VELOCITY 0

// convenient conversions
#define MM_TO_KM 1000000UL
#define MM_TO_M 1000UL
#define M_TO_KM 1000UL

#define MILLIS_TO_MICROS 1000UL
#define MICROS_TO_SECONDS 1000000UL
#define SECONDS_TO_MINUTES 60UL
#define SECONDS_TO_HOURS 3600UL
#define MS_TO_SECONDS 1000

#define KMPH_TO_MPS (10.0/36.0)

// hardware debounce timings
#define SENSOR_DEBOUNCE_PERIOD_MS 10UL
#define BUTTON_CONSTANT_STATE_MS 70UL

// data sizes
#define MAX_SIZE_OF_ERR_MSG 50
#define MAX_SIZE_OF_DIR_PATH 20
#define MAX_SIZE_OF_FILE_NAME 30
#define MAX_SIZE_OF_FILE_PATH (MAX_SIZE_OF_DIR_PATH + MAX_SIZE_OF_FILE_NAME)
// max size is this value so that a new file is created every 5 minutes for an average speed of 21 kmph
#define MAX_FILE_SIZE_BYTES 12800

// immage defines
#define BLACK_PIXEL 0
#define WHITE_PIXEL 1

#define DOT_STANDARD_SIZE 15
#define DOT_STANDARD_OFFSET 3
#define EMPTY_SPACE_FOR_DOT 10
#define DOT_SLANT_RATE_PX 5

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

// serial pins -> connector layout: (ON/OFF switch, USB port, rail), TX, GND, RX, (Other rail)
#define TX_SERIAL GPIO_NUM_17
#define RX_SERIAL GPIO_NUM_16

// pins for e-paper display
#define RST GPIO_NUM_27             // External reset, low active
#define BUSY GPIO_NUM_35            // Busy status output, high active

// data packet to send to menu
struct TripData
{
    uint32_t m_magnetDetections;
    // micros
    uint64_t m_rideStart;
    // micros
    int64_t m_latestDetectionTime;
    // micros
    int64_t m_previousDetectionTime;

    // KMPH
    double m_currentVelocity;
    // KMPH
    double m_previousVelocity;
    // KMPH
    double m_tripAvgVelocity;   

    TripData()
    {
        m_magnetDetections = 0; 
        m_rideStart = 0;
        m_latestDetectionTime = 0;
        m_previousDetectionTime = 0;

        m_currentVelocity = 0;
        m_previousVelocity = 0;
        m_tripAvgVelocity = 0;
    }

    bool operator== (const TripData p_rhs)
    {
        if(this->m_magnetDetections == p_rhs.m_magnetDetections
            && this->m_rideStart == p_rhs.m_rideStart
            && this->m_latestDetectionTime == p_rhs.m_latestDetectionTime
            && this->m_previousDetectionTime == p_rhs.m_previousDetectionTime

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
            && this->m_latestDetectionTime == p_rhs.m_latestDetectionTime
            && this->m_previousDetectionTime == p_rhs.m_previousDetectionTime

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
            this->m_latestDetectionTime = p_rhs.m_latestDetectionTime;
            this->m_previousDetectionTime = p_rhs.m_previousDetectionTime;
            
            this->m_currentVelocity = p_rhs.m_currentVelocity;
            this->m_previousVelocity = p_rhs.m_previousVelocity;
            this->m_tripAvgVelocity = p_rhs.m_tripAvgVelocity;
        }
        return *this;
    }
};

#endif