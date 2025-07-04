#ifndef DISPLAY_TASK_H
#define DISPLAY_TASK_H

// utility header for easy matrix and display opperations

#include <Arduino.h>
#include "numbersFont.h"
#include "signsMatrix.h"
#include "stampsFont.h"
#include "generalUtility.h"

#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>

//////// display logic
#define NUMBER_OF_UPDATES 4

#define DISPLAY_WIDTH 200
#define DISPLAY_HEIGHT 200       

#define DIGIT_ONE_SHIFT 8
#define WHOLE_NUMBER_ADJUSTMENT (DIGIT_ONE_SHIFT + 5)

// display rotation
#define ORIENTATION_HORIZONTAL 0
#define ORIENTATION_VERTICAL 1
#define ORIENTATION_HORIZONTAL_REVERSED 2
#define ORIENTATION_VERTICAL_REVERSED 3

extern GxEPD2_BW<GxEPD2_150_BN, GxEPD2_150_BN::HEIGHT> g_display;

extern uint8_t g_matrixToDisplay[DISPLAY_WIDTH][DISPLAY_HEIGHT];

/// @brief this function sends the appropriate commands to reset the ePaper display
void resetPanel();

/// @brief displays an immage with the same number of pixels as the ePaper display
///     it writes to the display RAM all of the pixels
///
/// @param p_immageBuffer buffer containing pixel information for the display - 1 for white, 0 for black
///     it needs to be 200 X 200 size 
/// @param p_displayFast if true does a fast refresh to the screen, if false does a full refresh
void displayImmage(uint8_t p_immageBuffer[DISPLAY_WIDTH][DISPLAY_HEIGHT], bool p_displayFast = false);

/// @brief displays an immage with the same number of pixels as the ePaper display
///     it writes to the display RAM only the black pixels
///
/// @param p_immageBuffer buffer containing pixel information for the display - 1 for white, 0 for black
///     it needs to be 200 X 200 size 
/// @param p_displayFast if true does a fast refresh to the screen, if false does a full refresh
void displayBlackPixels(uint8_t p_immageBuffer[DISPLAY_WIDTH][DISPLAY_HEIGHT], bool p_displayFast = false);

/// @brief Computes the number of digits in a number (531 has 3 digits)
///
/// @param p_number the number whose digits to get
/// @return the number of digits it computes
int getNumberOfDigits(int p_number);

/// @brief simple pow function to compute the power of a number
///
/// @param p_number the number to get the power of
/// @param p_exponent the exponent of the power to get, default 2 as it is the most common
/// @return the computed power as an int. I only needed this function to work on ints
int intPow(int p_number, int p_exponent = 2);

/// @brief the one in my font is really thin and i felt the need to bring it closer to the 
///     number next to it if it is the first or last digit in a number. This function 
///     computes the offset my number needs in order to be displayed centered after this 
///     operation that prints the one closer 
///
/// @param p_number the number to check for ones at the beginning and end
/// @param p_digitsInNumber the number of digits in this number, as i already compute it 
///     ahead of this function anyway
/// @return the offset value this function computes 
///     (as int because i'm workig on pixels and there is no such a thig as 0.2 pixels) 
int getOneOffsetValue(int p_number, int p_digitsInNumber);

void addSignCentered(uint8_t p_immageBuffer[DISPLAY_WIDTH][DISPLAY_HEIGHT], const bool *p_sign_pointer, int p_width, int p_height, float p_scale = 1);

/// @brief overlays the number given by the user on the immage that is going to be displayed
///     using the custom font saved in flash
///     it also centers the number both horisontally and vertically 
///
/// @param p_immageBuffer the immage buffer to print the numbers on 
/// @param p_numberToWrite the number to be converted from int to saved font
/// @param p_OxImmageOffset ox offset to be able to print the immage a little to the right/left 
/// @param p_OyImmageOffset oy offset to be able to print the immage a little higher or lower 
/// @param p_scale used to scale the immage, make it bigger or smaller, as a percentage of the original
void addNumberCentered(uint8_t p_immageBuffer[DISPLAY_WIDTH][DISPLAY_HEIGHT], int p_numberToWrite, int p_OxImmageOffset = 0, int p_OyImmageOffset = 0, float p_scale = 1);

/// @brief same as addNumberCentered but made for floating point numbers. This means that it can display
///     any number of decimals for the number given to display
/// @param p_immageBuffer the immage buffer to print the numbers on 
/// @param p_numberToWrite the number to be converted from int to saved font
/// @param p_decimalsToShow the number of decimals to display 
/// @param p_OxImmageOffset ox offset to be able to print the immage a little to the right/left
/// @param p_OyImmageOffset oy offset to be able to print the immage a little highter or lower 
/// @param p_scale used to scale the immage, make it bigger or smaller, as a percentage of the original
void addDoubleCentered(uint8_t p_immageBuffer[DISPLAY_WIDTH][DISPLAY_HEIGHT], double p_numberToWrite, int p_decimalsToShow = 1, int p_OxImmageOffset = 0, int p_OyImmageOffset = 0, float p_scale = 1);


/// @brief overlays the "stamp" given by the user on the immage that is going to be displayed
///     using the custom immages saved in flash
///     it also centers the immage both horisontally and vertically 
///
/// @param p_immageBuffer the immage buffer to print the numbers on 
/// @param p_numberToWrite the number to be converted from int to saved font
/// @param p_OxImmageOffset ox offset to be able to print the immage a little to the right/left 
/// @param p_OyImmageOffset oy offset to be able to print the immage a little higher or lower 
/// @param p_scale used to scale the immage, make it bigger or smaller, as a percentage of the original 
void addStampCentered(uint8_t p_immageBuffer[DISPLAY_WIDTH][DISPLAY_HEIGHT], int p_stampToWrite, int p_OxImmageOffset = 0, int p_OyImmageOffset = 0, float p_scale = 1);

/// @brief sets everythin in the immage buffer to white (1)
///     (immage that is going to be displayed on the screen)
///
/// @param p_immageBuffer the immage 
void clearImmage(uint8_t p_immageBuffer[DISPLAY_WIDTH][DISPLAY_HEIGHT]);

/// @brief returns true if the value is higher than the lower bound, it included, 
///     and lower than the upper bound, excluding it
///
/// @param p_value the value to check for 
/// @param p_lowerBound the lower bound value
/// @param p_upperBound the upper bound value
/// @return 
bool checkValueWithinBounds(int p_value, int p_lowerBound, int p_upperBound);

#endif