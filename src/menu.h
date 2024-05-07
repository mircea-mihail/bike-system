#ifndef MENU_H
#define MENU_H

// menu class meant to :
//      * store information that needs to be displayed,
//      * store menu state depending on user input 
//      * modify the matrix according to user input and latest information

// #include <numbersFont.h>
#include <displayTask.h>
#include "generalUtility.h"

#define TOP_FONT_OFFSET 20
#define SIDE_FONT_OFFSET 10
#define RIGHT_SIDE_FONT_OFFSET 25

extern GxEPD2_BW<GxEPD2_150_BN, GxEPD2_150_BN::HEIGHT> g_display;

enum mainMenuStates 
{
    Speed, 
    Acceleration
};

enum submenuStates 
{
    Instant, 
    TripAvg,
    BothCombined 
};

class Menu
{
private:
    // menu and submenu states
    mainMenuStates m_mainMenuState;
    submenuStates m_submenuState;

    // relevant information regarding to things that need to be displayed
    TripData m_tripData;
    bool m_changedState;

public:
    Menu(mainMenuStates p_mainMenuState = Speed, submenuStates p_submenuState = Instant)
        : m_mainMenuState(p_mainMenuState), m_submenuState(p_submenuState) 
    {
        m_changedState = true;
    };
    
    void getImmage(uint8_t p_matrixToDisplay[DISPLAY_WIDTH][DISPLAY_HEIGHT]);
    
    void update(TripData p_currentVelocity);

    void nextMenuState(bool p_shouldChangeState);
    
    void nextSubmenuState();

    void resetChangedState();

    bool getChangedState();

};

#endif