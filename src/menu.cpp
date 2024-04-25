#include "menu.h"

void Menu::enterSpeedSubmenu()
{
    
}

void Menu::getImmage(uint8_t p_matrixToDisplay[DISPLAY_WIDTH][DISPLAY_HEIGHT])
{
    switch(m_mainMenuState)
    {
        case Speed:
            switch(m_submenuState)
            {
                case Instant:
                    clearImmage(p_matrixToDisplay);
                    addNumberCentered(p_matrixToDisplay, m_currentVelocity);
                    break;

                case TripAvg:
                    clearImmage(p_matrixToDisplay);
                    addNumberCentered(p_matrixToDisplay, m_averageTripVelocity);
                    break;

                case BothCombined: 
                    clearImmage(p_matrixToDisplay);
                    addNumberCentered(p_matrixToDisplay, m_currentVelocity, 0, 50, 0.7);
                    addNumberCentered(p_matrixToDisplay, m_averageTripVelocity, 0, -50, 0.7);
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

void Menu::update(uint8_t p_currentVelocity)
{
    m_currentVelocity = p_currentVelocity;
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