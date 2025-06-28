#include <opencv2/opencv.hpp> 

#include <iostream>
#include <chrono>
#include <thread>

// serial
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include <filesystem>

#include "utility.h"
#include "detect_signs.h"

void load_templates(std::vector<cv::Mat> &p_templates)
{
	cv::Mat buf = cv::imread(std::string(STOP_PATH), cv::IMREAD_COLOR);
	p_templates.push_back(buf);
	buf = cv::imread(std::string(NO_BIKES_PATH), cv::IMREAD_COLOR);
	p_templates.push_back(buf);
	buf = cv::imread(std::string(CROSSING_PATH), cv::IMREAD_COLOR);
	p_templates.push_back(buf);
	buf = cv::imread(std::string(WRONG_WAY_PATH), cv::IMREAD_COLOR);
	p_templates.push_back(buf);
}

void detect_dir_images(std::string p_input_dir)
{
    const std::vector<std::string> validExtensions = {".jpg", ".jpeg", ".png", ".bmp", ".tiff"};
	
	if (p_input_dir[p_input_dir.length() - 1] == '/')
	{
		p_input_dir.pop_back();
	}

	std::string ouptut_dir = p_input_dir + "_solved";

	if (! std::filesystem::exists(ouptut_dir)) 
	{
        if (! std::filesystem::create_directory(ouptut_dir))
		{
            std::cerr << "Failed to create directory!" << std::endl;
			return ;
		}
	}
	double avg_score = 0;
	double avg_duration = 0;
	uint32_t images_cout = 0;
	uint32_t scored_images_count = 0;
	std::vector<cv::Mat> templates;
	load_templates(templates);

    for (const auto& entry : std::filesystem::directory_iterator(p_input_dir)) {
        // Get file path and extension
        const auto& path = entry.path();
        std::string extension = path.extension().string();

        // Convert extension to lowercase for comparison
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

        // Check if the file has a valid image extension
        if (std::find(validExtensions.begin(), validExtensions.end(), extension) != validExtensions.end()) {
            std::cout << "Opening image: " << path.string() << std::endl;
            cv::Mat image = cv::imread(path.string());

            if (! image.data) {
                std::cerr << "Failed to load image: " << path.string() << std::endl;
                continue;
            }

			cv::Point2i image_size = cv::Point2i(IMAGE_WIDTH, IMAGE_HEIGHT);
			cv::resize(image, image, cv::Size(image_size));

			std::chrono::time_point start = std::chrono::high_resolution_clock::now();
			int signs_found = detect_signs(image, templates);
			std::chrono::time_point end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double, std::milli> duration = end - start;

			avg_duration += duration.count();

			images_cout ++;
			std::cout << "took " << duration.count() << " milis\n";

			std::string output_path = ouptut_dir + "/" + path.filename().string();
			cv::imwrite(output_path, image);
        }
    }
	std::cout << "\nOn average it took " << avg_duration / images_cout << " milis\n";

}

void detect_from_name(std::string p_image_name)
{
	std::vector<cv::Mat> templates;
	load_templates(templates);

	cv::Mat image; 
	image = cv::imread(p_image_name); 

	if (!image.data) { 
		std::cerr << "Failed to load image: " << p_image_name << std::endl;
		return; 
	} 
	cv::Point2i image_size = cv::Point2i(IMAGE_WIDTH, IMAGE_HEIGHT);
	cv::resize(image, image, cv::Size(image_size));

	std::chrono::time_point start = std::chrono::high_resolution_clock::now();

	int signs = detect_signs(image, templates);

	std::chrono::time_point end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> duration = end - start;

	std::cout << "detect gw took " << duration.count() << " ms" << std::endl;

	show_pic(image);
}

int detect_pic(cv::Mat &p_pic, std::vector<cv::Mat> &p_templates)
{
	std::vector<cv::Mat> templates;
	load_templates(templates);

	cv::Point2i image_size = cv::Point2i(IMAGE_WIDTH, IMAGE_HEIGHT);
	cv::resize(p_pic, p_pic, cv::Size(image_size));

	std::chrono::time_point start = std::chrono::high_resolution_clock::now();

	int signs = detect_signs(p_pic, templates);

	std::chrono::time_point end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> duration = end - start;

	// show_pic(p_pic, "img");
	return signs;
}

int take_picture(cv::Mat &p_pic, cv::VideoCapture &p_camera)
{
	// Capture a p_pic from the camera
	p_camera >> p_pic;

        // Check if the p_pic was captured successfully
	if (p_pic.empty()) {
		std::cerr << "Error: Failed to capture a p_pic.\n";
		return -1;
	}

    return 0;
}

void save_pic(cv::Mat &p_pic, std::string p_output_dir, int32_t p_pic_idx)
{
	cv::Mat pic_to_save;
	cv::resize(p_pic, pic_to_save, cv::Size(cv::Point2i(SAVED_IMG_WIDTH, SAVED_IMG_HEIGHT)));

	cv::imwrite(p_output_dir + std::to_string(p_pic_idx) + ".jpg", pic_to_save);
}

int get_most_important_sign(int p_detections)
{
    if((p_detections & (1 << STOP)) != 0) 
		return STOP;

    if((p_detections & (1 << GIVE_WAY)) != 0) 
		return GIVE_WAY;

    if((p_detections & (1 << CROSSING)) != 0) 
		return CROSSING;

    if((p_detections & (1 << NO_BIKES)) != 0) 
		return NO_BIKES;

    if((p_detections & (1 << WRONG_WAY)) != 0) 
		return WRONG_WAY;

	return NO_SIGN;
}

void configure_serial(termios &options, int fd)
{
    tcgetattr(fd, &options);

    // Set baud rate 
    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);

    // 8 data bits, no parity, 1 stop bit (8N1)
    options.c_cflag &= ~PARENB; // no parity
    options.c_cflag &= ~CSTOPB; // 1 stop bit
    options.c_cflag &= ~CSIZE; 
    options.c_cflag |= CS8;     // 8 bits

    // Disable hardware flow control
    options.c_cflag &= ~CRTSCTS;

    // Enable receiver, ignore modem control lines
    options.c_cflag |= (CLOCAL | CREAD);

    // Raw input/output mode
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_iflag &= ~(IXON | IXOFF | IXANY);
    options.c_oflag &= ~OPOST;

    // Apply settings
    tcsetattr(fd, TCSANOW, &options);
}

void detection_loop()
{
	#ifdef IN_RASPI
		cv::VideoCapture camera(
		  "libcamerasrc ! "
		  "video/x-raw,width=1280,height=960,framerate=30/1 ! "
		  "videoconvert ! "
		  "appsink drop=true max-buffers=1"
		);
		// cv::VideoCapture camera("libcamerasrc ! video/x-raw,width=1280,height=960,framerate=30/1 ! videoconvert ! appsink");

		camera.set(cv::CAP_PROP_FRAME_WIDTH, IMAGE_WIDTH);
		camera.set(cv::CAP_PROP_FRAME_HEIGHT, IMAGE_HEIGHT);
		camera.set(cv::CAP_PROP_FPS, 30); 

		// try to open the video until you can
		while (!camera.isOpened()) {
			camera.open("/dev/video2");
			std::cout << "failed to video capture" << std::endl;
			sleep(0.1);
		}
		// warm up the camera a little
		cv::Mat temp;
		for (int i = 0; i < 10; ++i) {
			camera.read(temp);
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		
		const char* serial_port = "/dev/serial0"; 
		// Open serial port for read/write, not controlling terminal, no delay
		int fd = open(serial_port, O_RDWR | O_NOCTTY | O_NDELAY);
		while (fd == -1) {
			fd = open(serial_port, O_RDWR | O_NOCTTY | O_NDELAY);
			std::cout << "failed to open serial" << std::endl;
			sleep(0.1);
		}
		struct termios options;
		configure_serial(options, fd);
	#else
		// for USB cameras
		cv::VideoCapture camera(0);
		if (!camera.isOpened()) {
			std::cerr << "Error: Could not open the camera.\n";
			return;
		}
	#endif

	cv::Mat pic;
	std::vector<cv::Mat> templates;
	load_templates(templates);

	int32_t pic_idx = 0;
	int32_t ctrl_pic_idx = 0;
	int32_t maybe_idx = 0;

	std::chrono::time_point loop_start_time = std::chrono::high_resolution_clock::now();
	int32_t loop_time_freq = 100;
	int32_t current_iteration = 0;

	std::chrono::time_point control_pic_start = std::chrono::high_resolution_clock::now();

	std::string output_dir = std::string(LOG_PIC_PATH).append("/detections/");
	std::string ctrl_output_dir = std::string(LOG_PIC_PATH).append("/control/");

	while(true)
	{
		if (! std::filesystem::exists(output_dir)) 
		{
			if (! std::filesystem::create_directory(output_dir))
			{
				std::cerr << "Failed to create directory!" << std::endl;
				break;
			}
		}
		if (! std::filesystem::exists(ctrl_output_dir)) 
		{
			if (! std::filesystem::create_directory(ctrl_output_dir))
			{
				std::cerr << "Failed to create directory!" << std::endl;
				break;
			}
		}

		std::chrono::time_point take_pic_start = std::chrono::high_resolution_clock::now();

		if(take_picture(pic, camera) == 0)
		{
			std::cout << "took pic" << std::endl;
			std::chrono::time_point take_pic_end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double, std::milli> take_pic_duration = take_pic_end - take_pic_start;
			cv::medianBlur(pic, pic, 3);

			int signs_found = detect_pic(pic, templates);

			// todo 
			// maybe_idx = detection_score > 0 ? maybe_idx+1 : 0;
			// if(maybe_idx >= MIN_MAYBE_IDX)
			if(signs_found != 0)
			{
				std::cout << "\tdetections found in pic:" << std::endl;
				print_detections(signs_found);
				int sign_to_send = get_most_important_sign(signs_found);

				#ifdef IN_RASPI
					std::string message = std::to_string(sign_to_send) + "\n";
					ssize_t bytes_written = write(fd, message.c_str(), message.size());

					if (bytes_written < 0) {
						std::cerr << "Failed to write to serial port\n";
					}
					std::cout << "Wrote " << message << " to " << serial_port << "\n";
				#endif

				std::cout << "take picture took " << take_pic_duration.count() << " ms" << std::endl;
				std::chrono::duration<double, std::milli> detect_duration = std::chrono::high_resolution_clock::now() - take_pic_start;
				std::cout << "detection took " << detect_duration.count() << " ms" << std::endl;

				// show_pic(pic);
				save_pic(pic, output_dir, pic_idx);
				std::chrono::duration<double, std::milli> store_duration = std::chrono::high_resolution_clock::now() - take_pic_start;
				std::cout << "storing to flash took " << store_duration.count() << " ms" << std::endl;
	
				pic_idx ++;
			}

			std::chrono::time_point control_pic_end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double, std::milli> control_delta = control_pic_end - control_pic_start; 
			if(control_delta.count() > CONTROL_PIC_INTERVAL_MS)
			{
				control_pic_start = std::chrono::high_resolution_clock::now();
				std::cout << "took control pic..." << std::endl;

				save_pic(pic, ctrl_output_dir, ctrl_pic_idx);
				ctrl_pic_idx ++;
			}

			current_iteration += 1;
			if(current_iteration >= loop_time_freq)
			{
				std::chrono::time_point loop_end_time = std::chrono::high_resolution_clock::now();
				std::chrono::duration<double, std::milli> loop_time = loop_end_time - loop_start_time; 

				std::cout << "on avg the loop takes " << loop_time.count() / current_iteration << "ms" << std::endl;
				loop_start_time = std::chrono::high_resolution_clock::now();
				current_iteration = 0;
			}
		}
		else std::cout << "failed to take pic" << std::endl;
	}
	camera.release();
}

int main(int argc, char** argv) 
{ 
	std::vector<cv::Mat> template_vector;
	load_templates(template_vector);

	detection_loop();

	// if (argc != 2) { 
	// 	printf("usage: main_detect <Images Dir>\n"); 
	// 	return -1; 
	// } 
	// detect_dir_images(argv[1]);

	// detect_from_name(argv[1]);
	return 0; 
}
