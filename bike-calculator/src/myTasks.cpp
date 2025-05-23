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
            
            taskYIELD(); // yeld after update
        }   
    }
}

// list files, see which one starts with the highest number 
// followed by the filename i'm interested in
// check that file's size and if too big create a new one starting with a new number
// append to this file periodically checking size and creating an new one if needed
void writeToFileTask(void *p_args)
{
    FSInteraction::init();

    // csv files 
    const char writeDirPath[MAX_SIZE_OF_DIR_PATH] = "/data";
    
    // const char default
    const char errorFileName[MAX_SIZE_OF_FILE_NAME] = "hall_sensor_errors.txt";
    const char velocityAccFileName[MAX_SIZE_OF_FILE_NAME] = "speed_acc.txt";

    char errorCheckFilePath[MAX_SIZE_OF_FILE_PATH] = "/data/hall_sensor_errors.txt";
    char velocityAccFilePath[MAX_SIZE_OF_FILE_PATH];

    int currentFileIdx = FSInteraction::getLatestVersion(writeDirPath, velocityAccFileName);

    char fileVersion[MAX_SIZE_OF_FILE_NAME];
    snprintf(fileVersion, MAX_SIZE_OF_FILE_NAME, "%d", currentFileIdx);
    strcpy(velocityAccFilePath, writeDirPath);
    strcat(velocityAccFilePath, "/");
    strcat(velocityAccFilePath, fileVersion);
    strcat(velocityAccFilePath, "_");
    strcat(velocityAccFilePath, velocityAccFileName);

    if(FSInteraction::getFileSize(velocityAccFilePath) > MAX_FILE_SIZE_BYTES)
    {
        currentFileIdx ++;
        char fileVersion[MAX_SIZE_OF_FILE_NAME];
        snprintf(fileVersion, MAX_SIZE_OF_FILE_NAME, "%d", currentFileIdx);
        strcpy(velocityAccFilePath, writeDirPath);
        strcat(velocityAccFilePath, "/");
        strcat(velocityAccFilePath, fileVersion);
        strcat(velocityAccFilePath, "_");
        strcat(velocityAccFilePath, velocityAccFileName);
    }

    if(PRINT_CONTENTS_OF_ALL_FILES)
    {
        Serial.print("printing from error file:\n");
        FSInteraction::printFileContents(errorCheckFilePath);
        Serial.print("printing from monitor file:\n");
        FSInteraction::printFileContents(velocityAccFilePath);
        Serial.print("total file size: ");
        Serial.println(FSInteraction::getFileSize(velocityAccFilePath));
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
            char csvSpeedAccStartMsg[MAX_SIZE_OF_ERR_MSG] = "detection time micros\n";

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
                // record possible error                
                if(dataToWrite.m_currentVelocity >= SUSPICIOUS_SPEED_KMPH)
                {
                    snprintf(sendMessage, MAX_SIZE_OF_ERR_MSG, "%lu, %.2lf, %.2lf\n", 
                            (millis() / MS_TO_SECONDS), 
                            dataToWrite.m_currentVelocity, 
                            dataToWrite.m_previousVelocity);
                
                    FSInteraction::appendStringToFile(errorCheckFilePath, sendMessage);
                }

                // record time of latest detection
                // >>> len(str(pow(2, 64))) == 20 

                snprintf(sendMessage, MAX_SIZE_OF_ERR_MSG, "%" PRId64 "\n", 
                            dataToWrite.m_latestDetectionTime);
                            // (dataToWrite.m_currentVelocity - dataToWrite.m_previousVelocity) * (dataToWrite.m_currentVelocity + dataToWrite.m_previousVelocity) / (2 * WHEEL_PERIMETER_MM / MM_TO_KM) * M_TO_KM);
                FSInteraction::appendStringToFile(velocityAccFilePath, sendMessage);
                        
                Serial.print(sendMessage);

                // check file size and make a new file fi surpassed max acceptable file size
                if(FSInteraction::getFileSize(velocityAccFilePath) > MAX_FILE_SIZE_BYTES)
                {
                    currentFileIdx ++;
                    char fileVersion[MAX_SIZE_OF_FILE_NAME];
                    snprintf(fileVersion, MAX_SIZE_OF_FILE_NAME, "%d", currentFileIdx);
                    strcpy(velocityAccFilePath, writeDirPath);
                    strcat(velocityAccFilePath, "/");
                    strcat(velocityAccFilePath, fileVersion);
                    strcat(velocityAccFilePath, "_");
                    strcat(velocityAccFilePath, velocityAccFileName);
                }

                xSemaphoreGive(g_spiMutex);
                
            }  
            else
            {
                Serial.print("failed to write to task..\n");
            }

            previousData = dataToWrite;

        }

        taskYIELD(); 
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
        // todo daca spamez butonul de change menu, fortez un approximateVelocity care s-ar putea sa imi aproximeze ceva aiurea 
        // send informaiton to display task
        if(millis() - lastMeasure > SEND_MEASUREMENTS_PERIOD || menu.getChangedState())
        {
            lastMeasure = millis();

            // update the menu with the latest measured data
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
            // estimate real time trip data as if a measurement has just been made
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

        if(hwUtil.pressedMenuButton())
        {
            menu.nextMenuState();
        }
        
        taskYIELD();
    }
}