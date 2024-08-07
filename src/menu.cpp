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
    case Speed:
        switch(m_speedSubmenuState)
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

    case Misc:
        switch(m_miscSubmenuState)
        {   
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

        case Acceleration:
            g_display.getTextBounds("acc", 0, 0, &tbx, &tby, &tbw, &tbh);
            textX = ((DISPLAY_WIDTH - tbw) / 2) - tbx + SIDE_FONT_OFFSET;
            textY = TOP_FONT_OFFSET;

            g_display.setCursor(textX, textY);
            g_display.print("acc");

            g_display.getTextBounds("m/s^2", 0, 0, &tbx, &tby, &tbw, &tbh);
            textX = ((DISPLAY_WIDTH - tbw) / 2) - tbx - SIDE_FONT_OFFSET;
            textY = DISPLAY_HEIGHT - tbh- tby;

            g_display.setCursor(textX, textY);
            g_display.print("m/s^2");

            if(m_tripData.m_latestDetectionTime == m_tripData.m_previousDetectionTime)
            {
                currentAcceleration = 0;
            }
            else
            {
                // currentAcceleration = (m_tripData.m_currentVelocity - m_tripData.m_previousVelocity) * KMPH_TO_MPS / (2 * WHEEL_PERIMETER_MM / MM_TO_M);
                currentAcceleration = (m_tripData.m_currentVelocity - m_tripData.m_previousVelocity) * KMPH_TO_MPS / ((m_tripData.m_latestDetectionTime - m_tripData.m_previousDetectionTime) / (double)MICROS_TO_SECONDS);
            }

            clearImmage(p_matrixToDisplay);
            addDoubleCentered(p_matrixToDisplay, currentAcceleration, 2);

            break;

        case Force:
            g_display.getTextBounds("force", 0, 0, &tbx, &tby, &tbw, &tbh);
            textX = ((DISPLAY_WIDTH - tbw) / 2) - tbx + SIDE_FONT_OFFSET;
            textY = TOP_FONT_OFFSET;

            g_display.setCursor(textX, textY);
            g_display.print("force");

            g_display.getTextBounds("N", 0, 0, &tbx, &tby, &tbw, &tbh);
            textX = ((DISPLAY_WIDTH - tbw) / 2) - tbx - SIDE_FONT_OFFSET;
            textY = DISPLAY_HEIGHT - tbh- tby;

            g_display.setCursor(textX, textY);
            g_display.print("N");

            clearImmage(p_matrixToDisplay);
            addNumberCentered(p_matrixToDisplay, (int)(m_tripData.m_magnetDetections * WHEEL_PERIMETER_MM / MM_TO_M));

            break;

        default: 
            break;
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

void Menu::nextMenuState()
{
    if(m_mainMenuState == Speed)
    {
        m_mainMenuState = Misc;
    }
    else
    {
        m_mainMenuState = Speed;
    }

    m_changedState = true;
    m_speedSubmenuState = Instant;
    m_miscSubmenuState = Distance;
}

void Menu::nextSubmenuState()
{
    switch(m_mainMenuState)
    {
    case Speed:
        switch(m_speedSubmenuState)
        {
        case Instant:
            m_speedSubmenuState = TripAvg;
            m_changedState = true;
            break;

        case TripAvg:
            m_speedSubmenuState = BothCombined;
            m_changedState = true;
            break;
            
        case BothCombined:
            m_speedSubmenuState = Instant;
            m_changedState = true;
            break;
        
        default:
            break;
        }
        break;

    case Misc:
        switch(m_miscSubmenuState)
        {
        case Distance:
            m_miscSubmenuState = Time;
            m_changedState = true;
            break;

        case Time:
            m_miscSubmenuState = Acceleration;
            m_changedState = true;
            break;

        case Acceleration:
            m_miscSubmenuState = Force;
            m_changedState = true;
            break;

        case Force:
            m_miscSubmenuState = Distance;
            m_changedState = true;
            break;
        
        default: 
            break;
        }
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
        && this->m_speedSubmenuState == p_rhs.m_speedSubmenuState
        && this->m_miscSubmenuState == p_rhs.m_miscSubmenuState
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
        && this->m_speedSubmenuState == p_rhs.m_speedSubmenuState
        && this->m_miscSubmenuState == p_rhs.m_miscSubmenuState
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
        this->m_speedSubmenuState = p_rhs.m_speedSubmenuState;
        this->m_miscSubmenuState = p_rhs.m_miscSubmenuState;
        this->m_tripData = p_rhs.m_tripData;
    }
    return *this;
}