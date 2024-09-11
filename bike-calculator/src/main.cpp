// ePaper Library: https://github.com/ZinggJM/GxEPD2
// my include files
#include "displayTask.h"
#include "menu.h"
#include "hardwareUtility.h"
#include "FSInteraction.h"
#include "generalUtility.h"
#include "myTasks.h"

// tasks and task comms related
// lowest priority, no reason to have special priority all the time
#define DEFAULT_TASK_PRIORITY tskIDLE_PRIORITY
#define DEFAULT_TASK_STACK_SIZE 2048
#define MEASUREMENT_TASK_STACK_SIZE 8192
#define FILE_WRITING_TASK_STACK_SIZE 8192
#define QUEUE_SIZE 30

//////////////////////// Immage rela((ted
#define NUMBER_OX_OFFSET 20

void initPins()
{
    // ePaper display pins
    pinMode(CHANGE_SUBMENU_PIN, INPUT_PULLDOWN);
    pinMode(CHANGE_MENU_PIN, INPUT_PULLDOWN);
    pinMode(DISPLAY_CHIP_SELECT, OUTPUT);
    pinMode(SD_CHIP_SELECT, OUTPUT);
    pinMode(DATA_COMMAND, OUTPUT);
    pinMode(RST, OUTPUT);
    pinMode(BUSY, INPUT);
    pinMode(DATA_OUT, OUTPUT);
    pinMode(CLK, OUTPUT);

    // sensors:
    pinMode(HALL_SENSOR_PIN, INPUT_PULLDOWN);
    // digitalWrite(HALL_SENSOR_PIN, HIGH); // activate internal pullup resistor
}

GxEPD2_BW<GxEPD2_150_BN, GxEPD2_150_BN::HEIGHT> g_display(GxEPD2_150_BN(/*CS=D8*/ DISPLAY_CHIP_SELECT, /*DC=D3*/ DATA_COMMAND, /*RST=D4*/ RST, /*BUSY=D2*/ BUSY)); // DEPG0150BN 200x200, SSD1681, TTGO T5 V2.4.1

uint8_t g_matrixToDisplay[DISPLAY_WIDTH][DISPLAY_HEIGHT];
Menu g_menu;

xSemaphoreHandle g_menuMutex;
xSemaphoreHandle g_spiMutex;
QueueHandle_t g_tripDataQueue;

void setup()
{    
    initPins();
    Serial.begin(115200);

    g_tripDataQueue = xQueueCreate(QUEUE_SIZE, sizeof(TripData));
    if(g_tripDataQueue == NULL)
    {
        Serial.println("Error creating the queue\n");
    }
 
    g_spiMutex = xSemaphoreCreateMutex();
    g_menuMutex = xSemaphoreCreateMutex();

    TaskHandle_t writeToFsTask = NULL;
    xTaskCreate(writeToFileTask, "writeToFsTask", FILE_WRITING_TASK_STACK_SIZE, NULL, DEFAULT_TASK_PRIORITY, &writeToFsTask);

    TaskHandle_t displayTaskHandle = NULL;
    xTaskCreate(displayManagement, "display", DEFAULT_TASK_STACK_SIZE, NULL, DEFAULT_TASK_PRIORITY, &displayTaskHandle); 

    TaskHandle_t measurementTaskHandle = NULL;
    xTaskCreate(measurementTask, "measurement", MEASUREMENT_TASK_STACK_SIZE, NULL, DEFAULT_TASK_PRIORITY, &measurementTaskHandle);
}

// don't need the loop (everything takes place in custom tasks)
void loop()
{
    vTaskDelete(NULL);
};