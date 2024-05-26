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
#define MAX_SIZE_OF_FILE_PATH 50
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

/// @brief the display management task
///     * set up the display object from the epaper library and the 200 x 200 byte matrix
///     * every 180 seconds or so do a full refresh of the display (to ensure longevity)
///     * right after full refresh do a burst of fast refreshes to have the immage as clear as pssible 
///     * between every fast refresh leave a period of time to allow SPI usage to the filewriting 
///     * during normal operation, if the menu changed after reading the g_menu display immage 
///     * the display uses the menu object to show the appropriate thing on the ePaper display 
/// @param p_args no arguments
void displayManagement(void *p_args);

/// @brief the task that does the file writing
///     * init the file wrtiting library
///     * show the file contents on the serial and delete them according to the two defines 
///         (PRINT_CONTENTS_OF_ALL_FILES and RESET_FILES_AFTER_PRINT)
///     * end the task and exit if unable to init the file writing library
///     * otherwise append the files with the csv head in order to separate data from one trip to another
///     * receive new data on the communication queue and write it to the SD card if new
///     * SD card writing is guarded by mutex because both sd and display use SPI 
/// @param p_args no arguments
void writeToFileTask(void *p_args);

/// @brief the task that deals with the sensor measuring
///     * periodically send menu object with trip data
///     * instantly send new menu object when changing menu state by pressing menu button
///     * try to detect sensor as often as possible and update tripdata that is sent to the sd on detection
/// @param p_args no arguments
void measurementTask(void *p_args);

#endif