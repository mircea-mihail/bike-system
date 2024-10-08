#include <pictureTaking.h>

void configureSensor()
{
	sensor_t * s = esp_camera_sensor_get();
	// INITIAL OPTIMAL GUESS
	s->set_special_effect(s, 0); // 1 to 6 (0 - No Effect, 1 - Negative, 2 - Grayscale, 3 - Red Tint, 4 - Green Tint, 5 - Blue Tint, 6 - Sepia)

	// white balancing seems to ruin the pictures
	s->set_whitebal(s, 0);       // 0 = disable , 1 = enable
	s->set_awb_gain(s, 0);       // Auto White Balance enable (0 or 1)

	s->set_gain_ctrl(s, 1);      // auto gain on
	s->set_exposure_ctrl(s, 1);	 // auto exposure on
	// s->set_quality(s, 0);

	// s->set_aec_value(s, 0);

}

bool configureCamera()
{
    camera_config_t p_camConf;

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

	// p_camConf.frame_size = FRAMESIZE_QVGA;
	p_camConf.frame_size = FRAMESIZE_SVGA;
	p_camConf.jpeg_quality = 3;
	// p_camConf.fb_count = 1;

	if(psramFound())
	{
		// p_camConf.frame_size = FRAMESIZE_UXGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
		// p_camConf.jpeg_quality = 10;
		p_camConf.fb_count = 2;
		Serial.println("has psram");
	} 
	else 
	{
		// p_camConf.frame_size = FRAMESIZE_SVGA;
		// p_camConf.jpeg_quality = 12;
		p_camConf.fb_count = 1;
	}
    
	esp_err_t err = esp_camera_init(&p_camConf);
	if (err != ESP_OK) 
	{
		Serial.printf("Camera init failed with error 0x%x", err);
		return false;
	}

	configureSensor();

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
	int32_t pictureNumber = EEPROM.readInt(EEPROM_WRITE_ADDR);
	Serial.print("read int ");
	Serial.println(pictureNumber);
	pictureNumber += (int32_t)1;

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

	Serial.print("trying to write int ");
	Serial.println(pictureNumber);
	EEPROM.writeInt(EEPROM_WRITE_ADDR, pictureNumber);
	EEPROM.commit();
	Serial.println("write successful?");
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