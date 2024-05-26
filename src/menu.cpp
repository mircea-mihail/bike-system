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
                    g_display.setCursor(0, TOP_FONT_OFFSET);
                    g_display.print("s");
                    g_display.setCursor(0, 2*TOP_FONT_OFFSET);
                    g_display.print("p");
                    g_display.setCursor(0, 3*TOP_FONT_OFFSET);
                    g_display.print("e");
                    g_display.setCursor(0, 4*TOP_FONT_OFFSET);
                    g_display.print("e");
                    g_display.setCursor(0, 5*TOP_FONT_OFFSET);
                    g_display.print("d\n");
                    
                    g_display.setCursor(DISPLAY_WIDTH - RIGHT_SIDE_FONT_OFFSET, 7*TOP_FONT_OFFSET);
                    g_display.print("a");
                    g_display.setCursor(DISPLAY_WIDTH - RIGHT_SIDE_FONT_OFFSET, 8*TOP_FONT_OFFSET);
                    g_display.print("v");
                    g_display.setCursor(DISPLAY_WIDTH - RIGHT_SIDE_FONT_OFFSET, 9*TOP_FONT_OFFSET);
                    g_display.print("g");


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

bool Menu::getChangedState()
{
    return m_changedState;
}

bool Menu::operator== (const Menu p_rhs)
{
    if(this->m_changedState == p_rhs.m_changedState 
        && this->m_mainMenuState == p_rhs.m_mainMenuState
        && this->m_submenuState == p_rhs.m_submenuState
        && this->m_tripData == p_rhs.m_tripData)
    {
        return true;   
    }

    return false;
}

bool Menu::operator!= (const Menu p_rhs)
{
    if(this->m_changedState == p_rhs.m_changedState 
        && this->m_mainMenuState == p_rhs.m_mainMenuState
        && this->m_submenuState == p_rhs.m_submenuState
        && this->m_tripData == p_rhs.m_tripData)
    {
        return false;   
    }

    return true;
}

Menu& Menu::operator= (const Menu& p_rhs)
{
    if(this != &p_rhs)
    {
        this->m_changedState = p_rhs.m_changedState; 
        this->m_mainMenuState = p_rhs.m_mainMenuState;
        this->m_submenuState = p_rhs.m_submenuState;
        this->m_tripData = p_rhs.m_tripData;
    }
    return *this;
}