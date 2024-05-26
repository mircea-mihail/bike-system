#include "myTasks.h"

void displayManagement(void *p_args)
{    
    resetPanel();
    g_display.init(SERIAL_BITRATE, true, WAVESHARE_REFRESH_TIME_MS, false);
    
    clearImmage(g_matrixToDisplay);
    g_display.setRotation(ORIENTATION_VERTICAL);
    g_display.clearScreen();

    g_display.setFont(&FreeMonoBoldOblique9pt7b);
    g_display.setTextColor(GxEPD_BLACK);

    Menu menu;
    Menu prevMenuState;

    // one full white refresh
    clearImmage(g_matrixToDisplay);
    displayImmage(g_matrixToDisplay, false);

    bool needsFullRefresh;
    bool burstPrintAfterRefresh;

    bool doBurstRefresh = true;
    unsigned long lastBurstRefresh = millis();
    unsigned long lastFullRefresh = millis();
    while(true)
    {
        // only periodically do burst refresh (like every second for 6 seconds)
        if(millis() - lastBurstRefresh > BURST_REFRESH_TIMEOUT_MS)
        {
            lastBurstRefresh = millis();
            doBurstRefresh = true;
        }
        
        needsFullRefresh = (millis() - lastFullRefresh > FULL_EPAPER_REFRESH_PERIOD_MS);
        burstPrintAfterRefresh = (millis() - lastFullRefresh < PRINT_BURST_PERIOD_MS) && doBurstRefresh;

        // wait for as long as possible to receive the speed to print
        if (menu != g_menu && xSemaphoreTake(g_menuMutex, portMAX_DELAY))
        {
            menu = g_menu;
            xSemaphoreGive(g_menuMutex);
        }

        if(menu != prevMenuState || needsFullRefresh || burstPrintAfterRefresh)
        {
            doBurstRefresh = false;

            // setup display for refresh
            g_display.setFullWindow();
            g_display.firstPage();
            g_display.setTextSize(2);

            // enter menu and display the appropriate thing on the matrix then display it regardless of what it is

            if (xSemaphoreTake(g_spiMutex, portMAX_DELAY))
            {
                menu.getImmage(g_matrixToDisplay);

                // needs a full refresh
                if(needsFullRefresh)
                {
                    lastFullRefresh = millis();
                    displayBlackPixels(g_matrixToDisplay, false);
                }
                // fast refresh,
                else
                {   
                    displayBlackPixels(g_matrixToDisplay, true);
                }
                xSemaphoreGive(g_spiMutex);
            }

            prevMenuState = menu;    
        }   
    }
}

void writeToFileTask(void *p_args)
{
    // csv files 
    const char* errorCheckFilePath = "/data/hall_sensor_errors.txt";
    const char* velocityAccFilePath = "/data/speed_acc.txt";

    FSInteraction::init();

    if(PRINT_CONTENTS_OF_ALL_FILES)
    {
        Serial.print("printing from error file:\n");
        FSInteraction::printFileContents(errorCheckFilePath);
        Serial.print("printing from monitor file:\n");
        FSInteraction::printFileContents(velocityAccFilePath);
        
        if(RESET_FILES_AFTER_PRINT)
        {
            FSInteraction::deleteFileContents(errorCheckFilePath);
            FSInteraction::deleteFileContents(velocityAccFilePath);
        }
    }  

    if (FSInteraction::canWriteToFs()) 
    {  
        if (xSemaphoreTake(g_spiMutex, portMAX_DELAY))
        { 
            char csvErrStartMsg[MAX_SIZE_OF_ERR_MSG] = "time, velocity, previous velocity\n";
            char csvSpeedAccStartMsg[MAX_SIZE_OF_ERR_MSG] = "velocity, delta V\n";

            FSInteraction::appendStringToFile(errorCheckFilePath, csvErrStartMsg);
            FSInteraction::appendStringToFile(velocityAccFilePath, csvSpeedAccStartMsg);
            
            xSemaphoreGive(g_spiMutex);
        }
    }
    else
    {
        // no point to this task if it cannot write to the file system
        vTaskDelete(NULL); 
        return;
    }

    TripData dataToWrite, previousData;
    char sendMessage[MAX_SIZE_OF_ERR_MSG];

    while(true)
    {
        xQueueReceive(g_tripDataQueue, &dataToWrite, SEND_DATA_DELAY_TICKS);

        if(dataToWrite != previousData)
        {
            if (xSemaphoreTake(g_spiMutex, portMAX_DELAY))
            {
                Serial.print("about to write to task!\n");

                // record possible error                
                if(dataToWrite.m_currentVelocity >= SUSPICIOUS_SPEED_KMPH)
                {
                    snprintf(sendMessage, MAX_SIZE_OF_ERR_MSG, "%lu, %.2lf, %.2lf\n", 
                            (millis() / MS_TO_SECONDS), 
                            dataToWrite.m_currentVelocity, 
                            dataToWrite.m_previousVelocity);
                
                    FSInteraction::appendStringToFile(errorCheckFilePath, sendMessage);
                }

                // record speed and acceleration
                snprintf(sendMessage, MAX_SIZE_OF_ERR_MSG, "%.2lf, %.2lf\n", 
                            dataToWrite.m_currentVelocity, 
                            dataToWrite.m_currentVelocity - dataToWrite.m_previousVelocity);
                            // (dataToWrite.m_currentVelocity - dataToWrite.m_previousVelocity) * (dataToWrite.m_currentVelocity + dataToWrite.m_previousVelocity) / (2 * WHEEL_PERIMETER_MM / MM_TO_KM) * M_TO_KM);
                FSInteraction::appendStringToFile(velocityAccFilePath, sendMessage);
                        
                Serial.print(sendMessage);

                xSemaphoreGive(g_spiMutex);
            }  
            else
            {
                Serial.print("failed to write to task..\n");
            }

            previousData = dataToWrite;
        }
    }
}

void measurementTask(void *p_args)
{
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
                menu.resetChangedState();

                if (xSemaphoreTake(g_menuMutex, portMAX_DELAY))
                {
                    g_menu = menu;
                    xSemaphoreGive(g_menuMutex);
                }
            }
            else
            {
                TripData estimatedData = bikeCalc.approximateVelocity();
                menu.update(estimatedData);
                menu.resetChangedState();

                if (xSemaphoreTake(g_menuMutex, portMAX_DELAY))
                {
                    g_menu = menu;
                    xSemaphoreGive(g_menuMutex);
                }
            }
            
            sendingLatestSpeed = false;
        }

        // update the sensor ins 
        if(hwUtil.detectedSensor())
        {
            tripData = bikeCalc.recordDetection(); 
            
            // todo (not sure how long to wait, leaving 0 for now)
            xQueueSend(g_tripDataQueue, &tripData, SEND_DATA_DELAY_TICKS);

            sendingLatestSpeed = true;        
            previousVelocity = tripData.m_currentVelocity;
            
        }

        if(hwUtil.pressedSubmenuButton())
        {
            menu.nextSubmenuState();
        }
        // taskYIELD();
    }
}