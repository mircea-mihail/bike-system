#ifndef HARDWARE_UTILITY_H
#define HARDWARE_UTILITY_H

#include <Arduino.h>
#include "generalUtility.h"

class HardwareUtility
{
private:

    struct button
    {
        int m_prevState = LOW;
        int m_prevCountedState = LOW;
        unsigned long m_prevTime = 0;

        // needed when the readings are rare and biased( as in the case for the hold ) 
        // -> prevState and prevCountedState don't get to even out as in the case with normal button reads
        void resetStates()
        {
            m_prevState = LOW;   
            m_prevCountedState = LOW;
            m_prevTime = 0;
        }
    };

    button m_sensor;
    button m_menuButton;
    button m_submenuButton;

    unsigned long m_lastJoystickSound = 0;

public:
    // bool debounceActiveLowSenor(int64_t p_lastSensorDetectionTime, const int p_sensorPin);
    bool detectedSensor();

    bool pressedMenuButton();

    bool pressedSubmenuButton();
};


#endif