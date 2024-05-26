#include "hardwareUtility.h"

// no filtering on sensor reading -> all filtering is done in the speed recording area
bool HardwareUtility::detectedSensor()
{
    // unsigned long time = millis();
    int state = digitalRead(HALL_SENSOR_PIN);

    // if state just changed from low to high do something else wait for state change
    if(m_sensor.m_prevState != state)
    {
        // if detected magnet and it's been a while since last reading then probabily valid state read
        // if(state == LOW && m_sensor.m_prevTime - time > SENSOR_DEBOUNCE_PERIOD_MS)
        if(state == LOW)
        {
            // m_sensor.m_prevTime = time;
            m_sensor.m_prevState = state;
            return true;
        }
    
        // m_sensor.m_prevTime = time;
    }
    
    m_sensor.m_prevState = state;
    return false;
}

bool HardwareUtility::pressedMenuButton()
{
    unsigned long time = millis();
    int state = digitalRead(CHANGE_MENU_PIN);

    // if the button has a constant state
    if(state == m_menuButton.m_prevState)
    {
        // if the constant state has been kept for a while
        if(time - m_menuButton.m_prevTime > BUTTON_CONSTANT_STATE_MS && m_menuButton.m_prevCountedState != state)
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

bool HardwareUtility::pressedSubmenuButton()
{
    unsigned long time = millis();
    int state = digitalRead(CHANGE_SUBMENU_PIN);

    // if the button has a constant state
    if(state == m_submenuButton.m_prevState)
    {
        // if the constant state has been kept for a while
        if(time - m_submenuButton.m_prevTime > BUTTON_CONSTANT_STATE_MS && m_submenuButton.m_prevCountedState != state)
        {
            m_submenuButton.m_prevCountedState = state;

            if(state == HIGH)
            {
                m_submenuButton.m_prevState = state;
                return true;
            }
        }
    }
    else
    {
        m_submenuButton.m_prevTime = time;
    }

    m_submenuButton.m_prevState = state;
    
    return false;
}