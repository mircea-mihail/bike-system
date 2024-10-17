// tests results
///////////////////////////// for SVGA
//pic format- pics/sec and overshoot of second pic
// rgb 565 	- 2 pix 0.19 os 
// yuv 422 	- 2 pix 0.19 os 
// yuv 422 	- 2 pix 0.19 os 
// jpeg q10	- 25 pix no os 
// jpeg q3 	- 25 pix no os 
// jpeg w saving - 3 pix

///////////////////////////// jpeg no saving different screen sizes:
// res      - pics / sec
// cif 		- 51 
// hvga		- 25
// vga		- 25
// svga		- 25
// xga		- 11
// hd		- 11
// sxga		- 11
// uxga		- 11

#ifndef PICTURE_TAKING_H
#define PICTURE_TAKING_H

#include "FS.h"                // SD Card ESP32
#include "SD_MMC.h"            // SD Card ESP32
#include "soc/soc.h"           // Disable brownour problems
#include "soc/rtc_cntl_reg.h"  // Disable brownour problems
#include "driver/rtc_io.h"
#include <EEPROM.h>            // read and write from flash memory
// #include "img_converters.h"
#include <Arduino.h>

#include "esp_camera.h"
#include "imageDefines.h"

// Pin definition for CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

#define NO_CALIBRATION_PICS 50 

#define EEPROM_SIZE 8
#define EEPROM_WRITE_ADDR 1

/// @brief configures the esp camera by populating a config camera struct 
/// with the desired values and then initialising the camera using it
/// @return true on success, false on failure
bool configureCamera();

/// @brief takes NO_CALIBRATION_PICS and does nothing to them
/// to give time for the camera to adjust itself
void focusPicture();

/// @brief takes a picture and stores it to flash
void takePicture();

bool getPicture(uint8_t *p_pic);

///////////////////////////// TESTING

/// @brief takes multiple pictures, each one with a different sensor setting in order to compare them
void takeMultipleSettingsPics();

/// @brief takes pictures for one second and prints how many it managed to take
void oneSecTakePicture();

#endif