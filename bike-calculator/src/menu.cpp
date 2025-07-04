#include "menu.h"

void Menu::showStreetSign(uint8_t p_matrixToDisplay[DISPLAY_WIDTH][DISPLAY_HEIGHT], int p_signToShow)
{
    switch(p_signToShow)
    {
        case NO_SIGN:
            break;
        case CROSSING:
            clearImmage(p_matrixToDisplay);
            addSignCentered(p_matrixToDisplay, &g_crossing[0][0], CROSSING_WIDTH, CROSSING_HEIGHT, 2);
            break;
        case GIVE_WAY:
            clearImmage(p_matrixToDisplay);
            addSignCentered(p_matrixToDisplay, &g_give_way[0][0], GIVE_WAY_WIDTH, GIVE_WAY_HEIGHT, 2);
            break;
        case NO_BIKES:
            clearImmage(p_matrixToDisplay);
            addSignCentered(p_matrixToDisplay, &g_no_bikes[0][0], NO_BIKES_WIDTH, NO_BIKES_HEIGHT, 2);
            break;
        case STOP:
            clearImmage(p_matrixToDisplay);
            addSignCentered(p_matrixToDisplay, &g_stop[0][0], STOP_WIDTH, STOP_HEIGHT, 2);
            break;
        case NO_ENTRY:
            clearImmage(p_matrixToDisplay);
            addSignCentered(p_matrixToDisplay, &g_no_entry[0][0], NO_ENTRY_WIDTH, NO_ENTRY_HEIGHT, 2);
            break;
        default:
            break;
    }
}

void Menu::showMetric(uint8_t p_matrixToDisplay[DISPLAY_WIDTH][DISPLAY_HEIGHT])
{
    int16_t tbx, tby;
    uint16_t tbw, tbh;
    uint16_t textX;
    uint16_t textY;
    
    double distanceTravelled = 0;
    double minutesPassed = 0;
    double currentAcceleration = 0;

    switch(m_metricDisplayed)
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
    switch(m_metricDisplayed)
    {
    case InstantSpeed:
        m_metricDisplayed = TripAvg;
        m_changedState = true;
        break;
    case TripAvg:
        m_metricDisplayed = Time;
        m_changedState = true;
        break;
    case Time:
        m_metricDisplayed = Distance;
        m_changedState = true;
        break;
    case Distance:
        m_metricDisplayed = InstantSpeed;
        m_changedState = true;
        break;
    default:
        break;
    }
}

void Menu::prevMetricsPage()
{
    switch(m_metricDisplayed)
    {
    case InstantSpeed:
        m_metricDisplayed = Distance;
        m_changedState = true;
        break;
    case TripAvg:
        m_metricDisplayed = InstantSpeed;
        m_changedState = true;
        break;
    case Time:
        m_metricDisplayed = TripAvg;
        m_changedState = true;
        break;
    case Distance:
        m_metricDisplayed = Time;
        m_changedState = true;
        break;
    default:
        break;
    }
}

void Menu::defaultMetricsPage()
{
    m_metricDisplayed = InstantSpeed;
    m_changedState = true;
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
        && this->m_metricDisplayed == p_rhs.m_metricDisplayed
        && this->m_tripData == p_rhs.m_tripData)
    {
        return true;   
    }

    return false;
}

bool Menu::operator!= (const Menu p_rhs)
{
    if(this->m_changedState == p_rhs.m_changedState 
        && this->m_metricDisplayed == p_rhs.m_metricDisplayed
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
        this->m_metricDisplayed = p_rhs.m_metricDisplayed;
        this->m_tripData = p_rhs.m_tripData;
    }
    return *this;
}