#ifndef MY_TASKS_H
#define MY_TASKS_H

// adafrit GFX library
#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBoldOblique9pt7b.h>

#include <freertos/semphr.h>

// my includes
#include "menu.h"
#include "displayTask.h"
#include "generalUtility.h"
#include "FSInteraction.h"
#include "bikeCalc.h"
#include "hardwareUtility.h"

// for file reading to console
#define PRINT_CONTENTS_OF_ALL_FILES false
#define RESET_FILES_AFTER_PRINT     false

//////////////////////////////////////// displayManagement task defines
// waveshare E-Paper display config
#define SERIAL_BITRATE 115200
#define WAVESHARE_REFRESH_TIME_MS 2
// timing related
#define IMMAGE_REPRINT_MS 50
#define FULL_EPAPER_REFRESH_PERIOD_MS (180 * MS_TO_SECONDS)
#define PRINT_BURST_PERIOD_MS (7 * MS_TO_SECONDS)
#define BURST_REFRESH_TIMEOUT_MS (1 * MS_TO_SECONDS)

//////////////////////////////////////// writeToFile task
// data sizes
#define MAX_SIZE_OF_ERR_MSG 50
// timing related
#define SEND_DATA_DELAY_TICKS 0
// misc
#define SUSPICIOUS_SPEED_KMPH 55

//////////////////////////////////////// measurement task
// after full refresh keep printing for a bit to warm up the display
#define CONTINUOUS_PRINT_PERIOD_MS 15000 
#define SEND_MEASUREMENTS_PERIOD 1500

// spi_device_handle_t g_spi2;
extern uint8_t g_matrixToDisplay[DISPLAY_WIDTH][DISPLAY_HEIGHT];
extern Menu g_menu;

extern xSemaphoreHandle g_menuMutex;
extern xSemaphoreHandle g_spiMutex;
extern QueueHandle_t g_tripDataQueue;

void displayManagement(void *p_args);

void writeToFileTask(void *p_args);

void measurementTask(void *p_args);

#endif