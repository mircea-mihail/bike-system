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
    // todo move can write to FS here !

    // csv files 
    const char writeDirPath[MAX_SIZE_OF_DIR_PATH] = "/data";
    
    // const char default
    const char velocityAccFileName[MAX_SIZE_OF_FILE_NAME] = "speed_acc.txt";

    char velocityAccFilePath[MAX_SIZE_OF_FILE_PATH];

    int currentFileIdx = FSInteraction::getLatestVersion(writeDirPath, velocityAccFileName);

    // todo change file names
    // data/123_speeed_acc.txt
    char fileVersion[MAX_SIZE_OF_FILE_NAME];
    snprintf(fileVersion, MAX_SIZE_OF_FILE_NAME, "%d", currentFileIdx);
    strcpy(velocityAccFilePath, writeDirPath);
    strcat(velocityAccFilePath, "/");
    strcat(velocityAccFilePath, fileVersion);
    strcat(velocityAccFilePath, "_");
    strcat(velocityAccFilePath, velocityAccFileName);

    // make new file if the current one is too big
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

    // show contents of files on terminal
    if(PRINT_CONTENTS_OF_ALL_FILES)
    {
        Serial.print("printing from monitor file:\n");
        FSInteraction::printFileContents(velocityAccFilePath);
        Serial.print("total file size: ");
        Serial.println(FSInteraction::getFileSize(velocityAccFilePath));
        if(RESET_FILES_AFTER_PRINT)
        {
            FSInteraction::deleteFileContents(velocityAccFilePath);
        }
    }  

    // print the header to current file
    if (FSInteraction::canWriteToFs()) 
    {  
        if (xSemaphoreTake(g_spiMutex, portMAX_DELAY))
        { 
            //todo do i need previous velocity in logging?
            //todo add street sign idx
            std::string csvSpeedAccStartMsg = "detection time micros\n";

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
        // todo check if too many failed attempts to write to file happened, in which case kill the process (sd disconnected?)
        xQueueReceive(g_tripDataQueue, &dataToWrite, SEND_DATA_DELAY_TICKS);

        if(dataToWrite != previousData)
        {
            if (xSemaphoreTake(g_spiMutex, portMAX_DELAY))
            {
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
    unsigned long lastChangedMenu = 0;
    double previousVelocity = 0;
    bool sendingLatestSpeed = true;

    while(true)
    {
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
            // unless the estimation is not realistic (if no real detection was found, safe to say we're slowing down)
            else
            {
                TripData estimatedData = bikeCalc.approximateVelocity();
                if(estimatedData.m_currentVelocity > tripData.m_currentVelocity)
                {
                    estimatedData = tripData;
                }
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

        if(hwUtil.pressedPrevButton())
        {
            menu.prevMetricsPage();
            lastChangedMenu = millis();
        }
        if(hwUtil.pressedNextButton())
        {
            menu.nextMetricsPage();
            lastChangedMenu = millis();
        }
        if(millis() - lastChangedMenu > MAIN_MENU_TIMEOUT_MS && lastChangedMenu != 0)
        {
            menu.defaultMetricsPage();
            lastChangedMenu = 0;
        }
        
        taskYIELD();
    }
}

void serialCamTask(void *p_args)
{
    Serial.println("started serial cam task");
    int32_t signCode = -2;
    while(true)
    {
        if(g_camSerial.available())
        {
            Serial.println("about to get an int:");
            signCode = g_camSerial.parseInt();
            Serial.print("Got: ");
            Serial.println(signCode);
            taskYIELD();
        }
   }
}