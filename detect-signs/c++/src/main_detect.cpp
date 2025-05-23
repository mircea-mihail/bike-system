#include <opencv2/opencv.hpp> 

#include <iostream>
#include <chrono>
#include <thread>

#include <filesystem>

#include "utility.h"
#include "detect_signs.h"

#ifdef IN_RASPI
	#include <lccv.hpp>
#endif

void load_templates(std::vector<cv::Mat> &p_templates)
{
	cv::Mat buf = cv::imread(std::string(STOP_PATH), cv::IMREAD_COLOR);
	p_templates.push_back(buf);
	buf = cv::imread(std::string(NO_BIKES_PATH), cv::IMREAD_COLOR);
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
			float score = detect_gw_cv(image, templates);
			std::chrono::time_point end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double, std::milli> duration = end - start;

			avg_score += score;
			scored_images_count += 1 ? score > 0 : 0;
			avg_duration += duration.count();

			images_cout ++;
			std::cout << "took " << duration.count() << " milis\n";

			std::string output_path = ouptut_dir + "/" + path.filename().string();
			cv::imwrite(output_path, image);
        }
    }
	std::cout << "\nOn average it took " << avg_duration / images_cout << " milis\n";
	std::cout << "Avg score:" << avg_score / scored_images_count << std::endl;
	std::cout << "detected " << scored_images_count << " images" << std::endl;

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

	uint32_t gw_detected = detect_gw_cv(image, templates);

	std::cout << "found " << gw_detected << " give way signs" << std::endl;

	std::chrono::time_point end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> duration = end - start;

	std::cout << "detect gw took " << duration.count() << " ms" << std::endl;

	show_pic(image);
}

float detect_pic(cv::Mat &p_pic, std::vector<cv::Mat> &p_templates)
{
	std::vector<cv::Mat> templates;
	load_templates(templates);

	cv::Point2i image_size = cv::Point2i(IMAGE_WIDTH, IMAGE_HEIGHT);
	cv::resize(p_pic, p_pic, cv::Size(image_size));

	std::chrono::time_point start = std::chrono::high_resolution_clock::now();

	float score = detect_gw_cv(p_pic, templates);

	std::chrono::time_point end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> duration = end - start;


	if(score)
	{
		std::cout << "detect gw took " << duration.count() << " ms" << std::endl;

		#ifdef PRINT_STATS
			std::string img_desc = "Detection took " + std::to_string(int(duration.count())) + " millis";
			cv::Point desc_pt(10, 80);
			cv::putText(p_pic, img_desc, desc_pt, 1, 3, cv::Scalar(0, 0, 0), 7);   
			cv::putText(p_pic, img_desc, desc_pt, 1, 3, cv::Scalar(0, 255, 0), 3);   
		#endif
	}

	// show_pic(p_pic, "img");
	return score;
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


void detection_loop()
{
	#ifdef IN_RASPI
		lccv::PiCamera camera;
		
		camera.options->video_width=IMAGE_WIDTH;
		camera.options->video_height=IMAGE_HEIGHT;
		camera.options->verbose=false;
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

	#ifdef IN_RASPI
		camera.startVideo();
	#endif
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

		#ifdef IN_RASPI
			if(camera.getVideoFrame(pic, 1000))
		#else
			if(take_picture(pic, camera) == 0)
		#endif
		{
			std::chrono::time_point take_pic_end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double, std::milli> take_pic_duration = take_pic_end - take_pic_start;
			cv::medianBlur(pic, pic, 3);

			float detection_score = detect_pic(pic, templates);

			maybe_idx = detection_score > 0 ? maybe_idx+1 : 0;

			if(maybe_idx >= MIN_MAYBE_IDX)
			{
				std::cout << "take picture took " << take_pic_duration.count() << " ms" << std::endl;
				// show_pic(pic);
				save_pic(pic, output_dir, pic_idx);

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
	}
	#ifdef IN_RASPI
		camera.stopVideo();
	#else
		camera.release();
	#endif
}

int main(int argc, char** argv) 
{ 
	std::vector<cv::Mat> template_vector;
	load_templates(template_vector);

	// detection_loop();

	// if (argc != 2) { 
	// 	printf("usage: main_detect <Images Dir>\n"); 
	// 	return -1; 
	// } 
	// detect_dir_images(argv[1]);

	detect_from_name(argv[1]);
	return 0; 
}