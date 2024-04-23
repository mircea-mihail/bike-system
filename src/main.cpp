// ePaper Library: https://github.com/ZinggJM/GxEPD2

// adafrit GFX library
#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>
// my include files
#include <displayTask.h>

// pins
#define HALL_SENSOR_PIN GPIO_NUM_39

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
#define SEND_MEASUREMENTS_PERIOD 2000

//////////////////////// Speed coomputing related
// #define PI 3.1415
#define WHEEL_DIAMETER_MM 700UL
#define WHEEL_PERIMETER_MM (WHEEL_DIAMETER_MM * PI)

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

void doNormalRefreshSequence(unsigned long p_lastFullRefresh, int p_speedToPrint, int p_previousReceivedSpeed)
{
    if(millis() - p_lastFullRefresh < CONTINUOUS_PRINT_PERIOD_MS || p_previousReceivedSpeed != p_speedToPrint)
    {
        clearImmage(g_matrixToDisplay);
        addNumberCentered(g_matrixToDisplay, p_speedToPrint);    
        displayImmage(g_matrixToDisplay, true);
    }
}

// desired workflow:
//      * wait for message from sensor reading task,
//      * display received number 
//      * repeat

// important details:
//      * only update once per new speed value
//      * if received speed is the same as the current speed don't update
//      * after full refresh update continuously for some seconds or so "to warm" up pixels 
//      check bounds when printing anything to the screen
//      write rescale funtion to print the number as big or as small as you want
void displayManagement(void *args)
{    
    resetPanel();
    g_display.init(115200, true, 2, false);  //for Waveshare boards with 2ms reset pulse
    clearImmage(g_matrixToDisplay);
    g_display.setRotation(ORIENTATION_HORIZONTAL);
    g_display.clearScreen();

    int previousReceivedSpeed = 0;
    unsigned long lastFullRefresh = 0;

    while(true)
    {
        int speedToPrint = previousReceivedSpeed;
        // wait for as long as possible to receive the speed to print
        xQueueReceive(g_communicationQueue, &speedToPrint, SEND_DATA_DELAY_TICKS);
        // Serial.println("Received message");

        // needs a full refresh
        if(millis() - lastFullRefresh > FULL_EPAPER_REFRESH_PERIOD_MS)
        {
            lastFullRefresh = millis();
            clearImmage(g_matrixToDisplay);
            addNumberCentered(g_matrixToDisplay, speedToPrint);    
            displayImmage(g_matrixToDisplay, false);
         
            clearImmage(g_matrixToDisplay);
            displayImmage(g_matrixToDisplay, false);
        }
        // average refresh
        else
        {        
            doNormalRefreshSequence(lastFullRefresh, speedToPrint, previousReceivedSpeed);
        }

        previousReceivedSpeed = speedToPrint;
    }
}

#define MM_TO_KM 1000000UL
#define MILLIS_TO_MICROS 1000UL
#define MICROS_TO_SECONDS 1000000UL
#define SECONDS_TO_HOURS 3600UL
#define DEBOUNCE_PERIOD_MS 50UL


int getSpeedKmPerH(int64_t p_lastWheelDetectionTime)
{
    int64_t fullSpinDurationMicros = esp_timer_get_time() - p_lastWheelDetectionTime;
    int64_t computedSpeed = (WHEEL_PERIMETER_MM * SECONDS_TO_HOURS / fullSpinDurationMicros);
    return computedSpeed > 100 ? 99 : computedSpeed;
}

bool debounceActiveLowSenor(int64_t p_lastSensorDetectionTime, const int p_sensorPin)
{
    bool detectedActivity = !digitalRead(p_sensorPin);
    if(esp_timer_get_time() - p_lastSensorDetectionTime < (DEBOUNCE_PERIOD_MS * MILLIS_TO_MICROS))
    {
        return HIGH; // active low, return HIGH when innactive
    }
    return detectedActivity;
}

void measurementTask(void *args)
{
    unsigned long lastMeasure = 0;
    int speed = 0;
    bool hallHasSwitched = false;

    int64_t lastWheelDetectionTime = 0;
    bool sendingLatestSpeed = true;

    while(true)
    {
        if(millis() - lastMeasure > SEND_MEASUREMENTS_PERIOD)
        {
            lastMeasure = millis();
            //send command to the task

            if(sendingLatestSpeed)
            {
                xQueueSend(g_communicationQueue, &speed, SEND_DATA_DELAY_TICKS);
            }
            else
            {
                int approximatedSpeed = getSpeedKmPerH(lastWheelDetectionTime);
                xQueueSend(g_communicationQueue, &approximatedSpeed, SEND_DATA_DELAY_TICKS);
            }
            // Serial.println("Sent message");
            sendingLatestSpeed = false;
        }

        // the sensor is active low
        bool detectedMagnet = debounceActiveLowSenor(lastWheelDetectionTime, HALL_SENSOR_PIN);
        if(detectedMagnet && hallHasSwitched)
        {
            speed = getSpeedKmPerH(lastWheelDetectionTime);
            lastWheelDetectionTime = esp_timer_get_time();

            sendingLatestSpeed = true;
            hallHasSwitched = false;
        }
        if(!detectedMagnet)
        {
            hallHasSwitched = true;
        }
        // taskYIELD();
    }
}

void setup()
{
    initPins();
    Serial.begin(115200);

    g_communicationQueue = xQueueCreate(QUEUE_SIZE, sizeof(int));
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