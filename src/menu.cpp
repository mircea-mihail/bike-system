#include "menu.h"

void Menu::getImmage(uint8_t p_matrixToDisplay[DISPLAY_WIDTH][DISPLAY_HEIGHT])
{
    int16_t tbx, tby;
    uint16_t tbw, tbh;
    uint16_t textX;
    uint16_t textY;

    switch(m_mainMenuState)
    {
        case Speed:
            switch(m_submenuState)
            {
                case Instant:
                    g_display.getTextBounds("speed", 0, 0, &tbx, &tby, &tbw, &tbh);
                    textX = ((DISPLAY_WIDTH - tbw) / 2) - tbx + SIDE_FONT_OFFSET;
                    textY = TOP_FONT_OFFSET;

                    g_display.setCursor(textX, textY);
                    g_display.print("speed");

                    g_display.getTextBounds("KM/H", 0, 0, &tbx, &tby, &tbw, &tbh);
                    textX = ((DISPLAY_WIDTH - tbw) / 2) - tbx - SIDE_FONT_OFFSET;
                    textY = DISPLAY_HEIGHT - tbh- tby;

                    g_display.setCursor(textX, textY);
                    g_display.print("KM/H");

                    clearImmage(p_matrixToDisplay);
                    addNumberCentered(p_matrixToDisplay, (int)m_tripData.m_currentVelocity);
                    break;

                case TripAvg:
                    g_display.getTextBounds("avg speed", 0, 0, &tbx, &tby, &tbw, &tbh);
                    textX = ((DISPLAY_WIDTH - tbw) / 2) - tbx;
                    textY = TOP_FONT_OFFSET;

                    g_display.setCursor(textX, textY);
                    g_display.print("avg speed");

                    g_display.getTextBounds("KM/H", 0, 0, &tbx, &tby, &tbw, &tbh);
                    textX = ((DISPLAY_WIDTH - tbw) / 2) - tbx - SIDE_FONT_OFFSET;
                    textY = DISPLAY_HEIGHT - tbh - tby;

                    g_display.setCursor(textX, textY);
                    g_display.print("KM/H");

                    clearImmage(p_matrixToDisplay);
                    addNumberCentered(p_matrixToDisplay, (int)m_tripData.m_tripAvgVelocity);
                    break;

                case BothCombined: 
                    // g_display.setTextSize(2);
                    // g_display.print("Instant\nand\nAverage\nVelocity");

                    clearImmage(p_matrixToDisplay);
                    addNumberCentered(p_matrixToDisplay, m_tripData.m_currentVelocity, 0, -50, 0.7);
                    addNumberCentered(p_matrixToDisplay, m_tripData.m_tripAvgVelocity, 0, 50, 0.7);
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
            m_changedState = true;
            break;

        case TripAvg:
            m_submenuState = BothCombined;
            m_changedState = true;
            break;
            
        case BothCombined:
            m_submenuState = Instant;
            m_changedState = true;
            break;
        
        default:
            break;
    }
}

void Menu::resetChangedState()
{
    m_changedState = false;
}
