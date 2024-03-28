// GxEPD2_HelloWorld.ino by Jean-Marc Zingg
//
// Display Library example for SPI e-paper panels from Dalian Good Display and boards from Waveshare.
// Requires HW SPI and Adafruit_GFX. Caution: the e-paper panels require 3.3V supply AND data lines!
//
// Display Library based on Demo Example from Good Display: https://www.good-display.com/companyfile/32/
//
// Author: Jean-Marc Zingg
//
// Version: see library.properties
//
// Library: https://github.com/ZinggJM/GxEPD2

// Supporting Arduino Forum Topics (closed, read only):
// Good Display ePaper for Arduino: https://forum.arduino.cc/t/good-display-epaper-for-arduino/419657
// Waveshare e-paper displays with SPI: https://forum.arduino.cc/t/waveshare-e-paper-displays-with-spi/467865
//
// Add new topics in https://forum.arduino.cc/c/using-arduino/displays/23 for new questions and issues

// see GxEPD2_wiring_examples.h for wiring suggestions and examples
// if you use a different wiring, you need to adapt the constructor parameters!

// uncomment next line to use class GFX of library GFX_Root instead of Adafruit_GFX
//#include <GFX.h>

#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <GxEPD2_4C.h>
#include <GxEPD2_7C.h>
#include <Fonts/FreeMonoBold9pt7b.h>

#include <numbersFont.h>

// select the display class and display driver class in the following file (new style):
// #include "GxEPD2_display_selection_new_style.h"

// or select the display constructor line in one of the following files (old style):
// #include "GxEPD2_display_selection.h"
// #include "GxEPD2_display_selection_added.h"

// alternately you can copy the constructor from GxEPD2_display_selection.h or GxEPD2_display_selection_added.h to here
// e.g. for Wemos D1 mini:
// GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> display(GxEPD2_154_D67(/*CS=D8*/ SS, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4)); // GDEH0154D67

// for handling alternative SPI pins (ESP32, RP2040) see example GxEPD2_Example.ino

//////// pins used on board
#define DATA_OUT GPIO_NUM_23        // SPI MOSI pin, used designed pin for this on esp
#define CLK GPIO_NUM_18           // SPI SCK pin, used designed pin for this on esp
#define CHIP_SELECT GPIO_NUM_33     // SPI chip selection, low active
#define DATA_COMMAND GPIO_NUM_12    // Data/Command selection (high for data, low for command) -> am pus pinul de MISO

#define RST GPIO_NUM_27             // External reset, low active
#define BUSY GPIO_NUM_35            // Busy status output, high active

//////// display logic
#define NUMBER_OF_UPDATES 4

#define DISPLAY_WIDTH 200
#define DISPLAY_HEIGHT 200       

#define DIGIT_ONE_SHIFT 8
#define WHOLE_NUMBER_ADJUSTMENT (DIGIT_ONE_SHIFT / 3 * 2)

// generic defines
#define BLACK 0
#define WHITE 1

GxEPD2_BW<GxEPD2_150_BN, GxEPD2_150_BN::HEIGHT> display(GxEPD2_150_BN(/*CS=D8*/ CHIP_SELECT, /*DC=D3*/ DATA_COMMAND, /*RST=D4*/ RST, /*BUSY=D2*/ BUSY)); // DEPG0150BN 200x200, SSD1681, TTGO T5 V2.4.1

void helloWorld();

// spi_device_handle_t g_spi2;
uint8_t g_matrixToDisplay[DISPLAY_WIDTH][DISPLAY_HEIGHT];

void halfAndHalf(uint8_t g_matrixToDisplay[DISPLAY_WIDTH][DISPLAY_HEIGHT])
{
    for(int row = 0; row < DISPLAY_WIDTH; row++)
    {
        for(int col = 0; col < DISPLAY_HEIGHT; col ++)
        {
            g_matrixToDisplay[row][col] = row < DISPLAY_HEIGHT/2 ? 1 : 0;
        }
    }
}

void otherHalf(uint8_t g_matrixToDisplay[DISPLAY_WIDTH][DISPLAY_HEIGHT])
{
    for(int row = 0; row < DISPLAY_WIDTH; row++)
    {
        for(int col = 0; col < DISPLAY_HEIGHT; col ++)
        {
            g_matrixToDisplay[row][col] = col < DISPLAY_WIDTH/2 ? 1 : 0;
        }
    }
}

void resetPanel()
{
    // hardware reset
    gpio_set_level(RST, LOW);
    vTaskDelay(10/portTICK_PERIOD_MS);
    gpio_set_level(RST, HIGH);
    vTaskDelay(10/portTICK_PERIOD_MS);
}

const char HelloWorld[] = "Hello World!";

void displayImmage(uint8_t p_immageBuffer[DISPLAY_WIDTH][DISPLAY_HEIGHT], bool p_displayFast = false)
{   
    display.setFullWindow();
    display.firstPage();

    if(p_displayFast)
    {
        // display.setCursor(0, 0);
        for(int row = 0; row < DISPLAY_WIDTH; row += 1)
        {
            for(int col = 0; col < DISPLAY_HEIGHT; col += 1)
            {
                display.drawPixel(row, col, g_matrixToDisplay[row][col]);
            }
        }

        display.display(true);
    }
    else
    {
        do 
        {
            // display.setCursor(0, 0);
            for(int row = 0; row < DISPLAY_WIDTH; row+= 1)
            {
                for(int col = 0; col < DISPLAY_HEIGHT; col += 1)
                {
                    display.drawPixel(row, col, g_matrixToDisplay[row][col]);
                }
            }
        }while(display.nextPage());
    }  
}

void displayBlackPixels(uint8_t p_immageBuffer[DISPLAY_WIDTH][DISPLAY_HEIGHT], bool p_displayFast = false)
{   
    display.setFullWindow();
    display.firstPage();

    if(p_displayFast)
    {
        // display.setCursor(0, 0);
        for(int row = 0; row < DISPLAY_WIDTH; row += 1)
        {
            for(int col = 0; col < DISPLAY_HEIGHT; col += 1)
            {
                if(g_matrixToDisplay[row][col] == BLACK)
                {
                    display.drawPixel(row, col, g_matrixToDisplay[row][col]);
                }
            }
        }

        display.display(true);
    }
    else
    {
        do 
        {
            // display.setCursor(0, 0);
            for(int row = 0; row < DISPLAY_WIDTH; row += 1)
            {
                for(int col = 0; col < DISPLAY_HEIGHT; col += 1)
                {
                    if(g_matrixToDisplay[row][col] == BLACK)
                    {
                        display.drawPixel(row, col, g_matrixToDisplay[row][col]);
                    }
                }
            }
        }while(display.nextPage());
    }  
}

void helloWorld()
{
    display.setRotation(1);
    display.setFont(&FreeMonoBold9pt7b);
    display.setTextColor(GxEPD_BLACK);
    int16_t tbx, tby; uint16_t tbw, tbh;
    display.getTextBounds(HelloWorld, 0, 0, &tbx, &tby, &tbw, &tbh);
    // center the bounding box by transposition of the origin:
    uint16_t x = ((display.width() - tbw) / 2) - tbx;
    uint16_t y = ((display.height() - tbh) / 2) - tby;
    display.setFullWindow();
    display.firstPage();

    do
    {
        display.fillScreen(GxEPD_WHITE);
        display.setCursor(x, y);
    }
    while (display.nextPage());
}

int speed = 32;

void displayAgain()
{
    display.setRotation(1);
    display.setFont(&FreeMonoBold9pt7b);
    display.setTextColor(GxEPD_BLACK);
    int16_t tbx, tby; uint16_t tbw, tbh;
    
    char buffer[5];
    sprintf(buffer, "%d", speed);
    display.getTextBounds(buffer, 0, 0, &tbx, &tby, &tbw, &tbh);
    // center the bounding box by transposition of the origin:
    uint16_t x = ((display.width() - tbw) / 2) - tbx;
    uint16_t y = ((display.height() - tbh) / 2) - tby;
    
    display.setFullWindow();
    display.firstPage();

    display.setTextSize(3, 4);
    display.setCursor(x, y);

    display.print(buffer);
    display.display(true);
    
    speed --;
}

int getNumberOfDigits(int p_number)
{
    int numberOfDigits = 0;
    while(p_number/10 > 0)
    {
        numberOfDigits ++;
        p_number /= 10;
    }
    return numberOfDigits + 1;
} 

int intPow(int p_number, int p_exponent)
{
    int result = p_number;
    for(int i = 0; i < p_exponent - 1; i++)
    {
        result *= p_number;
    }

    return result;
}

int getOneOffsetValue(int p_number, int p_digitsInNumber)
{
    int offset = 0;
    if(p_number % 10 == 1)
    {
        offset += WHOLE_NUMBER_ADJUSTMENT;
    }
    if((p_number / intPow(10, p_digitsInNumber-1)) == 1)
    {
        offset -= WHOLE_NUMBER_ADJUSTMENT;
    }

    return offset;
}

void addNumberCentered(uint8_t p_immageBuffer[DISPLAY_WIDTH][DISPLAY_HEIGHT], int p_numberToWrite)
{   
    int digitsToWrite = getNumberOfDigits(p_numberToWrite);
    
    int xStartPos = (DISPLAY_WIDTH - digitsToWrite * FONT_IMAGE_WIDTH) / 2; // to center horisontally
    int yStartPos = (DISPLAY_HEIGHT - FONT_IMAGE_HEIGHT) / 2;

    // offset the whole number for recentering after "one" adjustment
    int wholeNumberOffset = getOneOffsetValue(p_numberToWrite, digitsToWrite);

    // clear previous immage
    for(int digitIdx = 0; digitIdx < digitsToWrite; digitIdx ++)
    {
        int digit = p_numberToWrite % 10;

        for(int row = yStartPos; row < yStartPos + FONT_IMAGE_HEIGHT; row++)
        {
            for(int col = xStartPos + FONT_IMAGE_WIDTH *(digitsToWrite - digitIdx - 1); col < xStartPos + FONT_IMAGE_WIDTH * (digitsToWrite - digitIdx - 1) + FONT_IMAGE_WIDTH; col ++)
            {
                p_immageBuffer[row][col] = WHITE; 
            }
        }
    }

    // takes around 0.8 microseconds to read one digit from flash
    for(int digitIdx = 0; digitIdx < digitsToWrite; digitIdx ++)
    {
        int digit = p_numberToWrite % 10;

        for(int row = yStartPos; row < yStartPos + FONT_IMAGE_HEIGHT; row++)
        {
            for(int col = xStartPos + FONT_IMAGE_WIDTH *(digitsToWrite - digitIdx - 1); col < xStartPos + FONT_IMAGE_WIDTH * (digitsToWrite - digitIdx - 1) + FONT_IMAGE_WIDTH; col ++)
            {
                int digitOffset = 0;           // used to mitigate the one being too far from the other digits

                if(digit == 1 && digitIdx == digitsToWrite - 1)
                {
                    digitOffset += DIGIT_ONE_SHIFT;
                }
                if(digit == 1 && digitIdx == 0)
                {
                    digitOffset -= DIGIT_ONE_SHIFT;
                }
                
                uint8_t pixelToWrite = pgm_read_byte(&g_digitVector[digit][row-yStartPos][col-xStartPos]);
                if(pixelToWrite == BLACK)
                {
                    p_immageBuffer[row][col + digitOffset + wholeNumberOffset] = pixelToWrite;

                }                
            }
        }
        p_numberToWrite /= 10;
    }

}

void clearDisplay(uint8_t p_immageBuffer[DISPLAY_WIDTH][DISPLAY_HEIGHT])
{
    for(int row = 0; row < DISPLAY_HEIGHT; row++)
    {
        for(int col = 0; col < DISPLAY_WIDTH; col ++)
        {
            p_immageBuffer[row][col] = 1;
        }
    }
}

void setup()
{
    //display.init(115200); // default 10ms reset pulse, e.g. for bare panels with DESPI-C02
    resetPanel();
    delay(3000);

    display.init(115200, true, 2, false); // USE THIS for Waveshare boards with "clever" reset circuit, 2ms reset pulse
    helloWorld();
    clearDisplay(g_matrixToDisplay);
    // display.hibernate();
}

// stuff left to do:
// writeNumberToMatrix function that converts in to font immage display
// connect the sensor and start collecting data

void loop()
{
// display number
    for(int i = 15; i < 22; i+=3)
    {
        delay(2000);
        clearDisplay(g_matrixToDisplay);
        addNumberCentered(g_matrixToDisplay, i);
        displayImmage(g_matrixToDisplay, false);
    }
};