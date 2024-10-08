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
	

#include "Arduino.h"

#include <imageEditing.h>
#include <pictureTaking.h>

// define the number of bytes you want to access
#define EEPROM_SIZE 1

int pictureNumber = 0;

#define FLASH_LED_PIN 4

#define NO_CALIBRATION_PICS 100

void initPins()
{
	pinMode(FLASH_LED_PIN, OUTPUT);
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

void oneSecTakePicture()
{
	Serial.print("started taking pics...");
	int picsTaken = 0;
	unsigned long start = millis();
	while(millis() - start  < 1000)
	{
		// Take Picture with Camera
		camera_fb_t *fb = esp_camera_fb_get();  
		if(!fb) 
		{
			Serial.println("Camera capture failed");
			return;
		}

		esp_camera_fb_return(fb); 

		picsTaken += 1;
	}
	Serial.print("took ");
	Serial.println(picsTaken);
	Serial.print("overshot by ");
	Serial.print(((millis() - start) - 1000) / 1000.0);

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

	takePicture();
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
