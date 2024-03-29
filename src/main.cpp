// ePaper Library: https://github.com/ZinggJM/GxEPD2

// adafrit GFX library
#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>
// my include files
#include <displayTask.h>

// lowest priority, no reason to have special priority all the time
#define DEFAULT_TASK_PRIORITY tskIDLE_PRIORITY
#define DEFAULT_TASK_STACK_SIZE 2048
#define QUEUE_SIZE 10

// timing related
#define MS_TO_SECONDS 1000
#define SEND_DATA_DELAY_TICKS 0
#define IMMAGE_REPRINT_MS 50
// #define FULL_EPAPER_REFRESH_PERIOD_MS (180 * MS_TO_SECONDS)
#define FULL_EPAPER_REFRESH_PERIOD_MS (20 * MS_TO_SECONDS)

GxEPD2_BW<GxEPD2_150_BN, GxEPD2_150_BN::HEIGHT> g_display(GxEPD2_150_BN(/*CS=D8*/ CHIP_SELECT, /*DC=D3*/ DATA_COMMAND, /*RST=D4*/ RST, /*BUSY=D2*/ BUSY)); // DEPG0150BN 200x200, SSD1681, TTGO T5 V2.4.1

// spi_device_handle_t g_spi2;
uint8_t g_matrixToDisplay[DISPLAY_WIDTH][DISPLAY_HEIGHT];

// using FreeRTOS queues to establish comms between tasks
QueueHandle_t g_communicationQueue;

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
    g_display.clearScreen();

    int previousReceivedSpeed = 0;
    unsigned long lastFullRefresh = 0;

    while(true)
    {
        int speedToPrint = 0;
        // wait for as long as possible to receive the speed to print
        xQueueReceive(g_communicationQueue, &speedToPrint, SEND_DATA_DELAY_TICKS);
        Serial.println("Received message");

        if(millis() - lastFullRefresh > FULL_EPAPER_REFRESH_PERIOD_MS)
        {
            lastFullRefresh = millis();
            clearDisplay(g_matrixToDisplay);
            addNumberCentered(g_matrixToDisplay, speedToPrint);    
            displayImmage(g_matrixToDisplay, false);
            g_display.clearScreen();
        }
        else
        {        
            clearDisplay(g_matrixToDisplay);
            addNumberCentered(g_matrixToDisplay, speedToPrint);    
            displayImmage(g_matrixToDisplay, true);

            // delay(IMMAGE_REPRINT_MS);
            // displayImmage(g_matrixToDisplay, true);
            // delay(IMMAGE_REPRINT_MS);
            // displayImmage(g_matrixToDisplay, true);
        }
        taskYIELD();
    }
}

void measurementTask(void *args)
{
    unsigned long lastMeasure = 0;
    const int reMeasureMillis = 4000;
    int speed = 0;

    while(true)
    {
        if(millis() - lastMeasure > reMeasureMillis)
        {
            lastMeasure = millis();
            //send command to the task
            xQueueSend(g_communicationQueue, &speed, SEND_DATA_DELAY_TICKS);
            Serial.println("Sent message");

            speed ++;
        }
        // Serial.print("doing work...\n");
        taskYIELD();
    }
}

void setup()
{
    initPins();

    g_communicationQueue = xQueueCreate(QUEUE_SIZE, sizeof(int));
    if(g_communicationQueue == NULL){
        Serial.println("Error creating the queue\n");
    }

    TaskHandle_t displayTaskHandle = NULL;
    xTaskCreate(displayManagement, "display", DEFAULT_TASK_STACK_SIZE, NULL, DEFAULT_TASK_PRIORITY, &displayTaskHandle); 

    TaskHandle_t measurementTaskHandle = NULL;
    xTaskCreate(measurementTask, "measurement", DEFAULT_TASK_STACK_SIZE, NULL, DEFAULT_TASK_PRIORITY, &measurementTaskHandle);
}

void loop(){};