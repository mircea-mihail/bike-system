#include "imageEditing.h"

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

			pixel px = get_rgb888_from_rgb565(image[i][j]);

			// Serial.print(px.red);
			// Serial.print(" ");
			// Serial.print(px.green);
			// Serial.print(" ");
			// Serial.print(px.blue);
			// Serial.print(" ");

			if(is_red(get_rgb888_from_rgb565(pixel16)))
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

pixel get_rgb888_from_rgb565(uint16_t p_pixel16)
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