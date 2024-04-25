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