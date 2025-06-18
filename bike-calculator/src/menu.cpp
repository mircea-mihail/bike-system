#include "menu.h"

void Menu::getImmage(uint8_t p_matrixToDisplay[DISPLAY_WIDTH][DISPLAY_HEIGHT])
{
    int16_t tbx, tby;
    uint16_t tbw, tbh;
    uint16_t textX;
    uint16_t textY;
    
    double distanceTravelled = 0;
    double minutesPassed = 0;
    double currentAcceleration = 0;

    switch(m_mainMenuState)
    {
    case InstantSpeed:
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
 
    case Time:
        g_display.getTextBounds("time", 0, 0, &tbx, &tby, &tbw, &tbh);
        textX = ((DISPLAY_WIDTH - tbw) / 2) - tbx + SIDE_FONT_OFFSET;
        textY = TOP_FONT_OFFSET;

        g_display.setCursor(textX, textY);
        g_display.print("time");

        g_display.getTextBounds("min", 0, 0, &tbx, &tby, &tbw, &tbh);
        textX = ((DISPLAY_WIDTH - tbw) / 2) - tbx - SIDE_FONT_OFFSET;
        textY = DISPLAY_HEIGHT - tbh- tby;

        g_display.setCursor(textX, textY);
        g_display.print("min");

        minutesPassed = esp_timer_get_time() / (double)MICROS_TO_SECONDS / (double)SECONDS_TO_MINUTES;
        clearImmage(p_matrixToDisplay);
        addDoubleCentered(p_matrixToDisplay, minutesPassed, 1);
        break;

    case Distance:
        g_display.getTextBounds("distance", 0, 0, &tbx, &tby, &tbw, &tbh);
        textX = ((DISPLAY_WIDTH - tbw) / 2) - tbx + SIDE_FONT_OFFSET;
        textY = TOP_FONT_OFFSET;

        g_display.setCursor(textX, textY);
        g_display.print("distance");

        g_display.getTextBounds("KM", 0, 0, &tbx, &tby, &tbw, &tbh);
        textX = ((DISPLAY_WIDTH - tbw) / 2) - tbx - SIDE_FONT_OFFSET;
        textY = DISPLAY_HEIGHT - tbh- tby;

        g_display.setCursor(textX, textY);
        g_display.print("KM");

        clearImmage(p_matrixToDisplay);
        distanceTravelled = m_tripData.m_magnetDetections * WHEEL_PERIMETER_MM / MM_TO_KM;
        if(distanceTravelled < 10)
        {
            addDoubleCentered(p_matrixToDisplay, distanceTravelled, 2);
        }
        else
        {
            addDoubleCentered(p_matrixToDisplay, distanceTravelled, 1);
        }
        break;

    default:
        break;
    }
}

void Menu::update(TripData p_tripData)
{
    m_tripData = p_tripData;
}

void Menu::nextMetricsPage()
{
    switch(m_mainMenuState)
    {
    case InstantSpeed:
        m_mainMenuState = TripAvg;
        m_changedState = true;
        break;
    case TripAvg:
        m_mainMenuState = Time;
        m_changedState = true;
        break;
    case Time:
        m_mainMenuState = Distance;
        m_changedState = true;
        break;
    case Distance:
        m_mainMenuState = InstantSpeed;
        m_changedState = true;
        break;
    default:
        break;
    }
}

void Menu::prevMetricsPage()
{
    switch(m_mainMenuState)
    {
    case InstantSpeed:
        m_mainMenuState = Distance;
        m_changedState = true;
        break;
    case TripAvg:
        m_mainMenuState = InstantSpeed;
        m_changedState = true;
        break;
    case Time:
        m_mainMenuState = TripAvg;
        m_changedState = true;
        break;
    case Distance:
        m_mainMenuState = Time;
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
        this->m_tripData = p_rhs.m_tripData;
    }
    return *this;
}