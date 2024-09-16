/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-cam-take-photo-save-microsd-card
  
  IMPORTANT!!! 
   - Select Board "AI Thinker ESP33-CAM"
   - GPIO 0 must be connected to GND to upload a sketch
   - After connecting GPIO 0 to GND, press the ESP32-CAM on-board RESET button to put your board in flashing mode
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

#include "esp_camera.h"
#include "Arduino.h"
#include "FS.h"                // SD Card ESP32
#include "SD_MMC.h"            // SD Card ESP32
#include "soc/soc.h"           // Disable brownour problems
#include "soc/rtc_cntl_reg.h"  // Disable brownour problems
#include "driver/rtc_io.h"
#include <EEPROM.h>            // read and write from flash memory

// define the number of bytes you want to access
#define EEPROM_SIZE 1

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

int pictureNumber = 0;

#define FLASH_LED_PIN 4

#define NO_CALIBRATION_PICS 10

void initPins()
{
	pinMode(FLASH_LED_PIN, OUTPUT);
}

bool configureCamera(camera_config_t &p_camConf)
{

	p_camConf.ledc_channel = LEDC_CHANNEL_0;
	p_camConf.ledc_timer = LEDC_TIMER_0;
	p_camConf.pin_d0 = Y2_GPIO_NUM;
	p_camConf.pin_d1 = Y3_GPIO_NUM;
	p_camConf.pin_d2 = Y4_GPIO_NUM;
	p_camConf.pin_d3 = Y5_GPIO_NUM;
	p_camConf.pin_d4 = Y6_GPIO_NUM;
	p_camConf.pin_d5 = Y7_GPIO_NUM;
	p_camConf.pin_d6 = Y8_GPIO_NUM;
	p_camConf.pin_d7 = Y9_GPIO_NUM;
	p_camConf.pin_xclk = XCLK_GPIO_NUM;
	p_camConf.pin_pclk = PCLK_GPIO_NUM;
	p_camConf.pin_vsync = VSYNC_GPIO_NUM;
	p_camConf.pin_href = HREF_GPIO_NUM;
	p_camConf.pin_sccb_sda = SIOD_GPIO_NUM;
	p_camConf.pin_sccb_scl = SIOC_GPIO_NUM;
	p_camConf.pin_pwdn = PWDN_GPIO_NUM;
	p_camConf.pin_reset = RESET_GPIO_NUM;
	p_camConf.xclk_freq_hz = 20000000;
	p_camConf.pixel_format = PIXFORMAT_JPEG; 

	p_camConf.frame_size = FRAMESIZE_CIF;
	p_camConf.jpeg_quality = 12;
	p_camConf.fb_count = 1;

	// if(psramFound())
	// {
	// 	p_camConf.frame_size = FRAMESIZE_UXGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
	// 	p_camConf.jpeg_quality = 10;
	// 	p_camConf.fb_count = 2;
	// } 
	// else 
	// {
	// 	p_camConf.frame_size = FRAMESIZE_SVGA;
	// 	p_camConf.jpeg_quality = 12;
	// 	p_camConf.fb_count = 1;
	// }

	esp_err_t err = esp_camera_init(&p_camConf);
	if (err != ESP_OK) 
	{
		Serial.printf("Camera init failed with error 0x%x", err);
		return false;
	}
	
	return true;
}

bool checkSD()
{
	if(!SD_MMC.begin())
	{
		Serial.println("SD Card Mount Failed");
		return false;
	}
	
	uint8_t cardType = SD_MMC.cardType();
	if(cardType == CARD_NONE)
	{
		Serial.println("No SD Card attached");
		return false;
	}
	return true;
}

void takePicture()
{
	// Take Picture with Camera
	camera_fb_t *fb = esp_camera_fb_get();  
	if(!fb) 
	{
		Serial.println("Camera capture failed");
		return;
	}

	esp_camera_fb_return(fb); 
	
	digitalWrite(FLASH_LED_PIN, LOW);
}

void takeAndStorePicture(int p_pictureNumber)
{
	// Take Picture with Camera
	camera_fb_t *fb = esp_camera_fb_get();  
	if(!fb) 
	{
		Serial.println("Camera capture failed");
		return;
	}

	// Path where new picture will be saved in SD Card
	String path = "/picture" + String(p_pictureNumber) +".jpg";

	fs::FS &fs = SD_MMC; 
	File file = fs.open(path.c_str(), FILE_WRITE);

	if(!file)
	{
		Serial.println("Failed to open file in writing mode");
	} 
	else 
	{
		file.write(fb->buf, fb->len); // payload (image), payload length
		Serial.printf("Saved file to path: %s\n", path.c_str());
	}
	file.close();
	esp_camera_fb_return(fb); 
	
	digitalWrite(FLASH_LED_PIN, LOW);
}

void setup() 
{
	initPins();

	WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector

	Serial.begin(115200);
	Serial.print("hello");
	// Serial.setDebugOutput(true);
	// Serial.println();
	
	if(! checkSD())
	{
		return;
	}

	camera_config_t camConf;
	if(! configureCamera(camConf))
	{
		return;
	}

	// initialize EEPROM with predefined size
	EEPROM.begin(EEPROM_SIZE);
	pictureNumber = EEPROM.read(0) + 1;

	for(int i = 0; i < NO_CALIBRATION_PICS; i++)
	{
		takePicture();
	}

	takeAndStorePicture(pictureNumber);

	EEPROM.write(0, pictureNumber);
	EEPROM.commit();

	// esp_deep_sleep_start();
}

void loop() 
{

}
