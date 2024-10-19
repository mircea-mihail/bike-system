#include "Arduino.h"

#include <imageEditing.h>
#include <pictureTaking.h>

// define the number of bytes you want to access
#define FLASH_LED_PIN 4


enum pixel_component{red = 0, green = 1, blue = 2};

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

uint8_t getColorComponent(uint8_t * p_pic, uint32_t p_height, uint32_t p_width, pixel_component p_px_comp)
{
	return p_pic[p_height * IMAGE_WIDTH * BYTES_PER_PIXEL + p_width + p_px_comp];
}

void pictureTaking(void *p_args)
{
	uint8_t *pic;
	pic = (uint8_t *)ps_malloc(IMAGE_HEIGHT * IMAGE_WIDTH * BYTES_PER_PIXEL);

	Serial.println("created task");
	while(true)
	{
		unsigned long start = millis();
		int pics_taken = 0;
		int dummy = 9;
		while(millis() - start < 1000)
		{
			// takePicture();
			getPicture(pic);
			pics_taken += 1;
			for(int i = 0; i < IMAGE_HEIGHT; i+=1)
			{
				for(int j = 0; j < IMAGE_WIDTH * BYTES_PER_PIXEL; j+= BYTES_PER_PIXEL)
				{
					dummy +=1;
				}
			}
		}
		Serial.print("\ntook ");
		Serial.println(pics_taken);
	}
}

void setup() 
{
	initPins();

	WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
	Serial.begin(115200);
	while(!Serial);

	digitalWrite(FLASH_LED_PIN, LOW);

	// if(!checkSD() )
	// {
	// 	return;
	// }
	if(!configureCamera())
	{
		return;
	}

	focusPicture();

	TaskHandle_t pictureTakingHandle = NULL;
    xTaskCreate(pictureTaking, "pictureTaking", 65536, NULL, tskIDLE_PRIORITY, &pictureTakingHandle); 

}

void loop() 
{
	vTaskDelete(NULL);
}
