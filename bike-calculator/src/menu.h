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
    Misc
};

enum speedSubmenuStates 
{
    Instant, 
    TripAvg,
    BothCombined 
};

enum miscSubmenuStates
{
    Distance,
    Time, 
    Force, 
    Acceleration
};

class Menu
{
private:
    // menu and submenu states
    mainMenuStates m_mainMenuState;
    speedSubmenuStates m_speedSubmenuState;
    miscSubmenuStates m_miscSubmenuState;

    // relevant information regarding to things that need to be displayed
    TripData m_tripData;
    bool m_changedState;

public:
    /// @brief constructor used to init the member variables
    /// @param p_mainMenuState the main menu state, used to know what to display
    /// @param p_speedSubmenuState the sub-menu state, used to know what to display
    Menu( mainMenuStates p_mainMenuState = Speed, 
          speedSubmenuStates p_speedSubmenuState = Instant, 
          miscSubmenuStates p_miscSubmenuState = Distance
        ) : m_mainMenuState(p_mainMenuState), m_speedSubmenuState(p_speedSubmenuState), m_miscSubmenuState(p_miscSubmenuState)
    {
        m_changedState = true;
    };
    
    /// @brief the part that does the actual displaying of the letters on the screen
    ///     and that writes to the pixel matrix the appropriate number to be then sent to the screen
    /// @param p_matrixToDisplay the 200 x 200 matrix that is then displayed to the screen
    void getImmage(uint8_t p_matrixToDisplay[DISPLAY_WIDTH][DISPLAY_HEIGHT]);
    
    /// @brief updates the tripdata member variable
    /// @param p_tripData value to put in the trip data member variable
    void update(TripData p_tripData);

    /// @brief updates the menu state if it should
    /// @param p_shouldChangeState only change state when this is true
    void nextMenuState();
    
    /// @brief updates the submenu state to the next one in order
    void nextSubmenuState();

    /// @brief resets the changedState member variable
    void resetChangedState();
    
    /// @brief used to get the state of the changedState member variable
    /// @return returns the state of the changedState member variable
    bool getChangedState();

    /// @brief overloads the == operator 
    /// @param p_rhs the right hand side Menu object to compare to
    /// @return returns the result of the comparison
    bool operator== (const Menu p_rhs);

    /// @brief overloads the == operator 
    /// @param p_rhs the right hand side Menu object to compare to
    /// @return returns the result of the comparison
    bool operator!= (const Menu p_rhs);

    /// @brief overloads the = operator
    /// @param p_rhs the right hand side Menu object
    /// @return returns the changed this refference
    Menu & operator= (const Menu& p_rhs);
};

#endif