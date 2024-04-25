// ePaper Library: https://github.com/ZinggJM/GxEPD2

// adafrit GFX library
#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>
// my include files
#include "displayTask.h"
#include "menu.h"
#include "bikeCalc.h"
#include "hardwareUtility.h"

// waveshare E-Paper display config
#define SERIAL_BITRATE 115200
#define WAVESHARE_REFRESH_TIME_MS 2

// tasks and task comms related
// lowest priority, no reason to have special priority all the time
#define DEFAULT_TASK_PRIORITY tskIDLE_PRIORITY
#define DEFAULT_TASK_STACK_SIZE 2048
#define QUEUE_SIZE 10

// timing related
#define MS_TO_SECONDS 1000
#define SEND_DATA_DELAY_TICKS 0
#define IMMAGE_REPRINT_MS 50
#define FULL_EPAPER_REFRESH_PERIOD_MS (180 * MS_TO_SECONDS)
// after full refresh keep printing for a bit to warm up the display
#define CONTINUOUS_PRINT_PERIOD_MS 15000 
#define SEND_MEASUREMENTS_PERIOD 1500

//////////////////////// Immage related
#define NUMBER_OX_OFFSET 20

GxEPD2_BW<GxEPD2_150_BN, GxEPD2_150_BN::HEIGHT> g_display(GxEPD2_150_BN(/*CS=D8*/ CHIP_SELECT, /*DC=D3*/ DATA_COMMAND, /*RST=D4*/ RST, /*BUSY=D2*/ BUSY)); // DEPG0150BN 200x200, SSD1681, TTGO T5 V2.4.1

// spi_device_handle_t g_spi2;
uint8_t g_matrixToDisplay[DISPLAY_WIDTH][DISPLAY_HEIGHT];

// using FreeRTOS queues to establish comms between tasks
QueueHandle_t g_communicationQueue;

void initPins()
{
    // ePaper display pins
    pinMode(CHANGE_SUBMENU_PIN, INPUT_PULLDOWN);
    pinMode(CHIP_SELECT, OUTPUT);
    pinMode(DATA_COMMAND, OUTPUT);
    pinMode(RST, OUTPUT);
    pinMode(BUSY, INPUT);
    pinMode(DATA_OUT, OUTPUT);
    pinMode(CLK, OUTPUT);

    // sensors:
    pinMode(HALL_SENSOR_PIN, INPUT_PULLDOWN);
    // digitalWrite(HALL_SENSOR_PIN, HIGH); // activate internal pullup resistor
}

// desired workflow:
//      * wait for message from sensor reading task,
//      * display received number 
//      * repeat

// important details:
//      * only update once per new speed value
//      * if received speed is the same as the current speed don't update
//      * after full refresh update continuously for some seconds or so to "warm up" pixels 
//      check bounds when printing anything to the screen
//      write rescale funtion to print the number as big or as small as you want
void displayManagement(void *args)
{    
    resetPanel();
    g_display.init(SERIAL_BITRATE, true, WAVESHARE_REFRESH_TIME_MS, false);
    
    clearImmage(g_matrixToDisplay);
    g_display.setRotation(ORIENTATION_HORIZONTAL);
    g_display.clearScreen();

    unsigned long lastFullRefresh = 0;
    Menu menu;

    // one full white refresh
    clearImmage(g_matrixToDisplay);
    displayImmage(g_matrixToDisplay, false);

    while(true)
    {
        // wait for as long as possible to receive the speed to print
        xQueueReceive(g_communicationQueue, &menu, SEND_DATA_DELAY_TICKS);

        // enter menu and display the appropriate thing on the matrix then display it regardless of what it is
        menu.getImmage(g_matrixToDisplay);

        // needs a full refresh
        if(millis() - lastFullRefresh > FULL_EPAPER_REFRESH_PERIOD_MS)
        {
            lastFullRefresh = millis();   
            displayImmage(g_matrixToDisplay, false);
        }
        // fast refresh,
        else
        {        
            displayImmage(g_matrixToDisplay, true);
        }
    }
}

void measurementTask(void *args)
{
    unsigned long lastMeasure = 0;
    uint8_t speed = 0;
    Menu menu;
    BikeCalc bikeCalc;

    int64_t lastWheelDetectionTime = 0;
    bool sendingLatestSpeed = true;

    HardwareUtility hwUtil;

    while(true)
    {
        // send latest speed every x seconds only if the queue is empty (the display has seen the previous speed sent)
        unsigned portBASE_TYPE queueLength = uxQueueMessagesWaiting(g_communicationQueue);
        if(millis() - lastMeasure > SEND_MEASUREMENTS_PERIOD && queueLength == 0)
        {
            lastMeasure = millis();

            if(sendingLatestSpeed)
            {
                menu.update(speed);
                xQueueSend(g_communicationQueue, &menu, SEND_DATA_DELAY_TICKS);
            }
            else
            {
                int approximatedSpeed = bikeCalc.approximateVelocity(lastWheelDetectionTime);
                menu.update(approximatedSpeed);
                xQueueSend(g_communicationQueue, &menu, SEND_DATA_DELAY_TICKS);
            }
            
            sendingLatestSpeed = false;
        }

        if(hwUtil.detectedSensor())
        {
            speed = bikeCalc.recordVelocity(lastWheelDetectionTime);
            lastWheelDetectionTime = esp_timer_get_time();

            sendingLatestSpeed = true;
        }

        // menu.nextSubmenuState(hwUtil.pressedMenuButton());
        // taskYIELD();
    }
}

void setup()
{
    initPins();
    Serial.begin(115200);

    g_communicationQueue = xQueueCreate(QUEUE_SIZE, sizeof(Menu));
    if(g_communicationQueue == NULL){
        Serial.println("Error creating the queue\n");
    }

    TaskHandle_t displayTaskHandle = NULL;
    xTaskCreate(displayManagement, "display", DEFAULT_TASK_STACK_SIZE, NULL, DEFAULT_TASK_PRIORITY, &displayTaskHandle); 
    Serial.print("init started");

    TaskHandle_t measurementTaskHandle = NULL;
    xTaskCreate(measurementTask, "measurement", DEFAULT_TASK_STACK_SIZE, NULL, DEFAULT_TASK_PRIORITY, &measurementTaskHandle);
}

void loop(){};