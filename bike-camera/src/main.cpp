#include "Arduino.h"

#include <imageEditing.h>
#include <pictureTaking.h>

// define the number of bytes you want to access
#define FLASH_LED_PIN 4

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

	if(! checkSD())
	{
		return;
	}
	if(!configureCamera())
	{
		return;
	}

	focusPicture();
	takePicture();
}

void loop() 
{
	// takePicture();
	if(findRedInPic())
	{
		Serial.println("found red");
		analogWrite(FLASH_LED_PIN, 1);
	}
	else
	{
		analogWrite(FLASH_LED_PIN, 0);
		Serial.println("no red...");
	}
}
