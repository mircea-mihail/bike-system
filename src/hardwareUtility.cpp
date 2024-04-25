#include "hardwareUtility.h"

// bool debounceActiveLowSenor(int64_t p_lastSensorDetectionTime, const int p_sensorPin)
// {
//     bool detectedActivity = !digitalRead(p_sensorPin);
//     if(esp_timer_get_time() - p_lastSensorDetectionTime < (DEBOUNCE_PERIOD_MS * MILLIS_TO_MICROS))
//     {
//         return HIGH; // active low, return HIGH when innactive
//     }
//     return detectedActivity;
// }

bool HardwareUtility::detectedSensor()
{
    unsigned long time = millis();
    int state = digitalRead(HALL_SENSOR_PIN);

    // if the button has a constant state
    if(state == m_sensor.m_prevState)
    {
        // if the constant state has been kept for a while
        if(time - m_sensor.m_prevTime > SENSOR_DEBOUNCE_PERIOD_MS && m_sensor.m_prevCountedState != state)
        {
            m_sensor.m_prevCountedState = state;

            if(state == HIGH)
            {
                m_sensor.m_prevState = state;
                return true;
            }
        }
    }
    else
    {
        m_sensor.m_prevTime = time;
    }

    m_sensor.m_prevState = state;
    
    return false;
}

bool HardwareUtility::pressedMenuButton()
{
    unsigned long time = millis();
    int state = digitalRead(CHANGE_SUBMENU_PIN);

    // if the button has a constant state
    if(state == m_menuButton.m_prevState)
    {
        // if the constant state has been kept for a while
        if(time - m_menuButton.m_prevTime > BUTTON_DEBOUNCE_PERIOD_MS && m_menuButton.m_prevCountedState != state)
        {
            m_menuButton.m_prevCountedState = state;

            if(state == HIGH)
            {
                m_menuButton.m_prevState = state;
                return true;
            }
        }
    }
    else
    {
        m_menuButton.m_prevTime = time;
    }

    m_menuButton.m_prevState = state;
    
    return false;
}