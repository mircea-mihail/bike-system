#include "Arduino.h"

#include <imageEditing.h>
#include <pictureTaking.h>

// define the number of bytes you want to access
#define FLASH_LED_PIN 4

uint16_t g_pic[IMAGE_HEIGHT][IMAGE_WIDTH];

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

void setup() 
{
	initPins();

	WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
	Serial.begin(115200);
	while(!Serial);

	digitalWrite(FLASH_LED_PIN, LOW);

	if(!checkSD() || !configureCamera())
	{
		return;
	}

	focusPicture();
	// takePicture();
	getPicture(g_pic);

	for(int i = 0; i < IMAGE_HEIGHT; i+=1)
	{
		for(int j = 0; j < IMAGE_WIDTH; j+=1)
		{
			pixel px = get_rgb888_from_rgb565(g_pic[i][j]);
			Serial.print(px.red);
			Serial.print(" ");
			Serial.print(px.green);
			Serial.print(" ");
			Serial.print(px.blue);
			Serial.print(", ");
		}
		Serial.print("\n");
	}
}

void loop() 
{
	delay(100);
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
