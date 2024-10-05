#include "esp_camera.h"
#include "Arduino.h"
#include "FS.h"                // SD Card ESP32
#include "SD_MMC.h"            // SD Card ESP32
#include "soc/soc.h"           // Disable brownour problems
#include "soc/rtc_cntl_reg.h"  // Disable brownour problems
#include "driver/rtc_io.h"
#include <EEPROM.h>            // read and write from flash memory

#include <imageEditing.h>

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

#define NO_CALIBRATION_PICS 25

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

	p_camConf.pixel_format = PIXFORMAT_RGB565; 
	p_camConf.frame_size = FRAMESIZE_QVGA;
	p_camConf.jpeg_quality = 10;
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

	sensor_t * s = esp_camera_sensor_get();
	// INITIAL OPTIMAL GUESS

	s->set_special_effect(s, 0); // 1 to 6 (0 - No Effect, 1 - Negative, 2 - Grayscale, 3 - Red Tint, 4 - Green Tint, 5 - Blue Tint, 6 - Sepia)

	// white balancing seems to ruin the pictures
	s->set_whitebal(s, 0);       // 0 = disable , 1 = enable
	s->set_awb_gain(s, 0);                        // Auto White Balance enable (0 or 1)

	s->set_gain_ctrl(s, 1);                       // auto gain on
	s->set_exposure_ctrl(s, 1);                   // auto exposure on

	// s->set_aec_value(s, 0);
	
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

void focusPicture()
{
	for(int i = 0; i < NO_CALIBRATION_PICS; i++)
	{
		// Take Picture with Camera
		camera_fb_t *fb = esp_camera_fb_get();  
		if(!fb) 
		{
			Serial.println("Camera capture failed");
			return;
		}

		esp_camera_fb_return(fb); 
	}
}

void takePicture()
{
	// digitalWrite(FLASH_LED_PIN, HIGH);

	EEPROM.begin(EEPROM_SIZE);
	int pictureNumber = EEPROM.read(0) + 1;

	// Take Picture with Camera
	camera_fb_t *fb = esp_camera_fb_get();  
	if(!fb) 
	{
		Serial.println("Camera capture failed");
		return;
	}

	// Path where new picture will be saved in SD Card
	String path = "/picture" + String(pictureNumber) +".bin";

	fs::FS &fs = SD_MMC; 
	File file = fs.open(path.c_str(), FILE_WRITE);

	if(!file)
	{
		Serial.println("Failed to open file in writing mode");
	} 
	else 
	{
		file.write(fb->buf, fb->len); // payload (image), payload length
		// Serial.printf("Saved file to path: %s\n", path.c_str());
	}
	file.close();

	esp_camera_fb_return(fb); 

	EEPROM.write(0, pictureNumber);
	EEPROM.commit();
}

void takeMultipleSettingsPics()
{
	sensor_t * s = esp_camera_sensor_get();

	s->set_ae_level(s, -2);
	takePicture();
	delay(100);

	for(int lvl = -2; lvl <= 2; lvl += 1)
	{
		// // the brighter the whiter the pixels are and the immage is more faded
		// s->set_brightness(s, 0);     // -2 to 2
		// // more contrast, clearer shapes, but too much might not be good
		// s->set_contrast(s, 0);       // -2 to 2
		// // more saturation, more distinct colors
		// s->set_saturation(s, 0);     // -2 to 2
		// // how much exposure to give the shot
		s->set_ae_level(s, lvl);     	 // -2 to 2

		takePicture();
		delay(100);
	}

	takePicture();
}

bool is_red(pixel p_px)
{
	if(		p_px.red > MIN_RED_FACTOR * p_px.blue
		&& 	p_px.red > MIN_RED_FACTOR * p_px.green
		&& 	p_px.red > MIN_RED_BRIGHTNESS)
	{
		return true;
	}
	return false;
}

pixel get_rgb(uint16_t p_pixel16)
{
	pixel px;
	px.red =   uint8_t((p_pixel16 & 0b1111100000000000) >> 11);
	px.green = uint8_t((p_pixel16 & 0b0000011111100000) >> 5);
	px.blue =  uint8_t((p_pixel16 & 0b0000000000011111));

	float max_red = 32;
	float max_green = 64;
	float max_blue = 32;
	float max_pixel = 255;

	px.red = uint8_t(max_pixel * (px.red / max_red));
	px.green = uint8_t(max_pixel * (px.green / max_green));
	px.blue = uint8_t(max_pixel * (px.blue / max_blue));

	return px;
}

bool findRedInPic()
{
	// Take Picture with Camera
	
	camera_fb_t *fb = esp_camera_fb_get();  

	if(!fb) 
	{
		Serial.println("Camera capture failed");
		return false;
	}

	uint16_t image[IMAGE_HEIGHT][IMAGE_WIDTH];
	uint16_t pixel16;

	for(int i = 0; i < IMAGE_HEIGHT; i++)
	{
		for(int j = 0; j < IMAGE_WIDTH; j++)
		{
			pixel16 = uint16_t(fb->buf[(i*IMAGE_HEIGHT + j) * 2]);
			pixel16 = pixel16 << 8;
			pixel16 = pixel16 | (uint16_t(fb->buf[(i*IMAGE_HEIGHT + j) * 2 + 1]));
			image[i][j] = pixel16;

			pixel px = get_rgb(image[i][j]);

			// Serial.print(px.red);
			// Serial.print(" ");
			// Serial.print(px.green);
			// Serial.print(" ");
			// Serial.print(px.blue);
			// Serial.print(" ");

			if(is_red(get_rgb(pixel16)))
			{
				esp_camera_fb_return(fb); 

				Serial.print("FOUND RED!!!\n");
				return true;
			}
		}
	}

	Serial.print("no red found...\n");

	esp_camera_fb_return(fb); 

	return false;
}

void setup() 
{
	initPins();

	WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
	Serial.begin(115200);
	while(!Serial);

	digitalWrite(FLASH_LED_PIN, LOW);

	if(! checkSD())
	{
		return;
	}
	camera_config_t camConf;
	if(! configureCamera(camConf))
	{
		return;
	}

	focusPicture();

	takeMultipleSettingsPics();
}

void loop() 
{
	// takePicture();
	// if(findRedInPic())
	// {
	// 	Serial.println("found red");
	// 	analogWrite(FLASH_LED_PIN, 1);
	// }
	// else
	// {
	// 	analogWrite(FLASH_LED_PIN, 0);
	// 	Serial.println("no red...");
	// }
}
