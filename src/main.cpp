// ePaper Library: https://github.com/ZinggJM/GxEPD2

// adafrit GFX library
#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>
// my include files
#include <displayTask.h>

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