// ePaper Library: https://github.com/ZinggJM/GxEPD2

// adafrit GFX library
#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBoldOblique9pt7b.h>

// my include files
#include "displayTask.h"
#include "menu.h"
#include "bikeCalc.h"
#include "hardwareUtility.h"
#include "FSInteraction.h"

// waveshare E-Paper display config
#define SERIAL_BITRATE 115200
#define WAVESHARE_REFRESH_TIME_MS 2

// tasks and task comms related
// lowest priority, no reason to have special priority all the time
#define DEFAULT_TASK_PRIORITY tskIDLE_PRIORITY
#define DEFAULT_TASK_STACK_SIZE 2048
#define MEASUREMENT_TASK_STACK_SIZE 8192
#define QUEUE_SIZE 10

// timing related
#define MS_TO_SECONDS 1000
#define SEND_DATA_DELAY_TICKS 0
#define IMMAGE_REPRINT_MS 50
#define FULL_EPAPER_REFRESH_PERIOD_MS (180 * MS_TO_SECONDS)
// after full refresh keep printing for a bit to warm up the display
#define CONTINUOUS_PRINT_PERIOD_MS 15000 
#define SEND_MEASUREMENTS_PERIOD 1500

//////////////////////// Immage rela((ted
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
    g_display.setRotation(ORIENTATION_VERTICAL);
    g_display.clearScreen();

    g_display.setFont(&FreeMonoBoldOblique9pt7b);
    g_display.setTextColor(GxEPD_BLACK);

    unsigned long lastFullRefresh = 0;
    Menu menu;

    // one full white refresh
    clearImmage(g_matrixToDisplay);
    displayImmage(g_matrixToDisplay, false);

    while(true)
    {
        // wait for as long as possible to receive the speed to print
        xQueueReceive(g_communicationQueue, &menu, SEND_DATA_DELAY_TICKS);
        
        // setup display for refresh
        g_display.setFullWindow();
        g_display.firstPage();
        g_display.setTextSize(2);

        // enter menu and display the appropriate thing on the matrix then display it regardless of what it is
        menu.getImmage(g_matrixToDisplay);

        // needs a full refresh
        if(millis() - lastFullRefresh > FULL_EPAPER_REFRESH_PERIOD_MS)
        {
            lastFullRefresh = millis();
            displayBlackPixels(g_matrixToDisplay, false);
        }
        // fast refresh,
        else
        {   
            displayBlackPixels(g_matrixToDisplay, true);
        }
    }
}

#define MAX_SIZE_OF_ERR_MSG 50
#define SPEED_THRESHOLD_VELOCITY_KMPH 55

// csv files 
const char* g_errorCheckFilePath = "/data/hall_sensor_errors.txt";
const char* g_velocityAccFilePath = "/data/speed_acc.txt";

// add filter that excludes speeds that surpass an acceleration threshold
// upload speeds to a /data/velocities.txt and acceleration between speeds to /data/accelerations.txt and analyse the data for acceleration filter
// periodically print an acc max for easier data analisys
// find a way to extract the data from the folders easier

void measurementTask(void *args)
{
    char sendMessage[MAX_SIZE_OF_ERR_MSG];

    if (FSInteraction::canWriteToFs()) 
    {   
        char csvErrStartMsg[MAX_SIZE_OF_ERR_MSG] = "time, velocity\n";
        char csvSpeedAccStartMsg[MAX_SIZE_OF_ERR_MSG] = "velocity, delta V, acceleration\n";

        FSInteraction::appendStringToFile(g_errorCheckFilePath, csvErrStartMsg);
        FSInteraction::appendStringToFile(g_velocityAccFilePath, csvSpeedAccStartMsg);
    }

    TripData tripData;
    Menu menu;
    BikeCalc bikeCalc;
    HardwareUtility hwUtil;

    unsigned long lastMeasure = 0;
    double previousVelocity = 0;
    bool sendingLatestSpeed = true;

    while(true)
    {
        // used to send informaiton to display task
        if(millis() - lastMeasure > SEND_MEASUREMENTS_PERIOD || menu.getChangedState())
        {
            lastMeasure = millis();

            if(sendingLatestSpeed)
            {
                menu.update(tripData);
                xQueueSend(g_communicationQueue, &menu, SEND_DATA_DELAY_TICKS);
                menu.resetChangedState();
            }
            else
            {
                TripData estimatedData = bikeCalc.approximateVelocity();
                menu.update(estimatedData);
                xQueueSend(g_communicationQueue, &menu, SEND_DATA_DELAY_TICKS);
                menu.resetChangedState();
            }
            
            sendingLatestSpeed = false;
        }

        // update the sensor ins 
        if(hwUtil.detectedSensor())
        {
            tripData = bikeCalc.recordDetection(); 
            sendingLatestSpeed = true;

            // record errors
            if(tripData.m_currentVelocity > SPEED_THRESHOLD_VELOCITY_KMPH)
            {
                snprintf(sendMessage, MAX_SIZE_OF_ERR_MSG, "%lu, %lf\n", millis()/MS_TO_SECONDS, tripData.m_currentVelocity);
                FSInteraction::appendStringToFile(g_errorCheckFilePath, sendMessage);
            }
            // record speed and acceleration
            snprintf(sendMessage, MAX_SIZE_OF_ERR_MSG, "%lf, %lf, %lf\n", 
                        tripData.m_currentVelocity, 
                        tripData.m_currentVelocity - previousVelocity,
                        (tripData.m_currentVelocity - previousVelocity) * (tripData.m_currentVelocity + previousVelocity) / (2 * WHEEL_PERIMETER_MM / MM_TO_KM));
            FSInteraction::appendStringToFile(g_velocityAccFilePath, sendMessage);
            
            previousVelocity = tripData.m_currentVelocity;
            
        }

        if(hwUtil.pressedSubmenuButton())
        {
            menu.nextSubmenuState();
        }
        // taskYIELD();
    }
}

#define PRINT_CONTENTS_OF_ALL_FILES false
#define RESET_FILES_AFTER_PRINT false

void setup()
{    
    initPins();
    Serial.begin(115200);
    
    g_communicationQueue = xQueueCreate(QUEUE_SIZE, sizeof(Menu));
    if(g_communicationQueue == NULL){
        Serial.println("Error creating the queue\n");
    }

    FSInteraction::init();

    if(PRINT_CONTENTS_OF_ALL_FILES)
    {
        Serial.print("printing from error file:\n");
        FSInteraction::printFileContents(g_errorCheckFilePath);
        
        Serial.print("printing from monitor file:\n");
        FSInteraction::printFileContents(g_velocityAccFilePath);
        
        if(RESET_FILES_AFTER_PRINT)
        {
            FSInteraction::deleteFileContents(g_errorCheckFilePath);
            FSInteraction::deleteFileContents(g_velocityAccFilePath);
        }
    }   
    else
    {
        TaskHandle_t displayTaskHandle = NULL;
        xTaskCreate(displayManagement, "display", DEFAULT_TASK_STACK_SIZE, NULL, DEFAULT_TASK_PRIORITY, &displayTaskHandle); 

        TaskHandle_t measurementTaskHandle = NULL;
        xTaskCreate(measurementTask, "measurement", MEASUREMENT_TASK_STACK_SIZE, NULL, DEFAULT_TASK_PRIORITY, &measurementTaskHandle);
    }
}

void loop(){};