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

public:
    /// @brief detects if the sensor changed state from 1 to 0 (active low sensor)
    /// @return returns true if the sensor is triggered
    bool detectedSensor();

    /// @brief debounces the menu button
    /// @return returns true if it has been pressed
    bool pressedNextButton();

    /// @brief debounces the submenu button
    /// @return returns true if it has been pressed
    bool pressedPrevButton();
};


#endif