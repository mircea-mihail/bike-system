#ifndef MENU_H
#define MENU_H

// menu class meant to :
//      * store information that needs to be displayed,
//      * store menu state depending on user input 
//      * modify the matrix according to user input and latest information

// #include <numbersFont.h>
#include <displayTask.h>

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
    uint8_t m_currentVelocity = 0;
    uint8_t m_averageTripVelocity = 0;

    void enterSpeedSubmenu();

public:
    Menu(mainMenuStates p_mainMenuState = Speed, submenuStates p_submenuState = Instant)
        : m_mainMenuState(p_mainMenuState), m_submenuState(p_submenuState) {};
    
    void getImmage(uint8_t p_matrixToDisplay[DISPLAY_WIDTH][DISPLAY_HEIGHT]);
    
    void update(uint8_t p_currentVelocity);

    void nextMenuState(bool p_shouldChangeState);
    
    void nextSubmenuState();

};

#endif