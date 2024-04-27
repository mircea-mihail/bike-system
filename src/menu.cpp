#include "menu.h"

void Menu::getImmage(uint8_t p_matrixToDisplay[DISPLAY_WIDTH][DISPLAY_HEIGHT])
{
    switch(m_mainMenuState)
    {
        case Speed:
            switch(m_submenuState)
            {
                case Instant:
                    clearImmage(p_matrixToDisplay);
                    addNumberCentered(p_matrixToDisplay, m_tripData.m_currentVelocity);
                    break;

                case TripAvg:
                    clearImmage(p_matrixToDisplay);
                    addNumberCentered(p_matrixToDisplay, m_tripData.m_tripAvgVelocity);
                    break;

                case BothCombined: 
                    clearImmage(p_matrixToDisplay);
                    addNumberCentered(p_matrixToDisplay, m_tripData.m_currentVelocity, 0, 50, 0.7);
                    addNumberCentered(p_matrixToDisplay, m_tripData.m_tripAvgVelocity, 0, -50, 0.7);
                    break;

                default:
                    break;
            }
            break;

        case Acceleration:
            break;

        default:
            break;
    }
}

void Menu::update(TripData p_tripData)
{
    m_tripData = p_tripData;
}

void Menu::nextMenuState(bool p_shouldChangeState)
{
    if(p_shouldChangeState)
    {
        if(m_mainMenuState == Speed)
        {
            m_mainMenuState = Acceleration;
        }
        else
        {
            m_mainMenuState = Speed;
        }
    }
}

void Menu::nextSubmenuState()
{
    // Serial.print("current submenu: ");
    // Serial.println(m_submenuState);
    switch(m_submenuState)
    {
        case Instant:
            m_submenuState = TripAvg;
            break;

        case TripAvg:
            m_submenuState = BothCombined;
            break;
            
        case BothCombined:
            m_submenuState = Instant;
            break;
        
        default:
            break;
    }
}