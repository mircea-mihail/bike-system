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
#include <Fonts/FreeMonoBold9pt7b.h>

#include <displayTask.h>

// select the display class and display driver class in the following file (new style):
// #include "GxEPD2_display_selection_new_style.h"

// or select the display constructor line in one of the following files (old style):
// #include "GxEPD2_display_selection.h"
// #include "GxEPD2_display_selection_added.h"

// alternately you can copy the constructor from GxEPD2_display_selection.h or GxEPD2_display_selection_added.h to here
// e.g. for Wemos D1 mini:
// GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> display(GxEPD2_154_D67(/*CS=D8*/ SS, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4)); // GDEH0154D67

// for handling alternative SPI pins (ESP32, RP2040) see example GxEPD2_Example.ino

// lowest priority, no reason to have special priority all the time
#define DEFAULT_TASK_PRIORITY tskIDLE_PRIORITY

#define DISPLAY_TASK_STACK_SIZE 2048

GxEPD2_BW<GxEPD2_150_BN, GxEPD2_150_BN::HEIGHT> g_display(GxEPD2_150_BN(/*CS=D8*/ CHIP_SELECT, /*DC=D3*/ DATA_COMMAND, /*RST=D4*/ RST, /*BUSY=D2*/ BUSY)); // DEPG0150BN 200x200, SSD1681, TTGO T5 V2.4.1

// spi_device_handle_t g_spi2;
uint8_t g_matrixToDisplay[DISPLAY_WIDTH][DISPLAY_HEIGHT];

void initPins()
{
    // ePaper display pins
    pinMode(CHIP_SELECT, OUTPUT);
    pinMode(DATA_COMMAND, OUTPUT);
    pinMode(RST, OUTPUT);
    pinMode(BUSY, INPUT);
    pinMode(DATA_OUT, OUTPUT);
    pinMode(CLK, OUTPUT);
}


// desired workflow:
//      * wait for message from sensor reading task,
//      * display received number 
//      * repeat
void displayManagement(void *args)
{    
    resetPanel();
    g_display.init(115200, true, 2, false);  //for Waveshare boards with 2ms reset pulse
    clearDisplay(g_matrixToDisplay);
    g_display.setRotation(ORIENTATION_HORIZONTAL);
   
    while(true)
    {
        g_display.clearScreen();

        // test display purposes
        for(int i = 0; i < 99; i+=1)
        {
            delay(500);
            clearDisplay(g_matrixToDisplay);
            addNumberCentered(g_matrixToDisplay, i);
        
            displayImmage(g_matrixToDisplay, true);
            delay(200);
            displayImmage(g_matrixToDisplay, true);
        }
    
        taskYIELD();
    }
}

void setup()
{
    initPins();

    TaskHandle_t displayTaskHandle = NULL;
    xTaskCreate(displayManagement, "display", DISPLAY_TASK_STACK_SIZE, NULL, DEFAULT_TASK_PRIORITY, &displayTaskHandle); 

}

void loop(){};