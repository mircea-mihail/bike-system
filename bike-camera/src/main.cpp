#include "Arduino.h"

#include <imageEditing.h>
#include <pictureTaking.h>

// define the number of bytes you want to access
#define FLASH_LED_PIN 4

uint8_t *g_pic;

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

uint16_t getPixel16(uint8_t *p_pic, uint32_t p_height, uint32_t p_width)
{
	return (p_pic[(p_height * IMAGE_WIDTH + p_width) * 2]) | (p_pic[(p_height * IMAGE_WIDTH + p_width) * 2 + 1]) << 8;
}

void setup() 
{
	initPins();

	WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
	Serial.begin(115200);
	while(!Serial);

	// Serial.print("Total heap: ");
	// Serial.println(ESP.getHeapSize());
	// Serial.print("Free heap:");
	// Serial.println(ESP.getFreeHeap());
	// Serial.print("Total PSRAM:");
	// Serial.println(ESP.getPsramSize());
	// Serial.print("Free PSRAM:");
	// Serial.println(ESP.getFreePsram());

	digitalWrite(FLASH_LED_PIN, LOW);

	// if(!checkSD() )
	// {
	// 	return;
	// }
	if(!configureCamera())
	{
		return;
	}

	g_pic = (uint8_t *)ps_malloc(IMAGE_HEIGHT * IMAGE_WIDTH * BYTES_PER_PIXEL);
	Serial.println("allocated pic");

	focusPicture();
	// takePicture();
	getPicture(g_pic);
	Serial.println("stored pic in psram");

	Serial.print("[");
	for(int i = 0; i < IMAGE_HEIGHT; i+=1)
	{
		Serial.print("[");
		for(int j = 0; j < IMAGE_WIDTH; j+=1)
		{
			pixel px = get_rgb888_from_rgb565(getPixel16(g_pic, i, j));

			Serial.print("[");

			Serial.print(px.red);
			Serial.print(", ");
			Serial.print(px.green);
			Serial.print(", ");
			Serial.print(px.blue);

			Serial.print("]");
			if(j != IMAGE_WIDTH - 1)
			{
				Serial.print(", ");
			}
		}
		Serial.print("]");
		if(i != IMAGE_HEIGHT - 1)
		{
			Serial.print(", ");
		}
		Serial.print("\n");
	}
	Serial.print("]");

	free(g_pic);
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
