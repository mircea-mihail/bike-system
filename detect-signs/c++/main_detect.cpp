#include <opencv2/opencv.hpp> 
#include <iostream>
#include <chrono>

#include <filesystem>

#include "utility.h"
#include "detect_signs.h"
#include "old_detect_signs.h"

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

    for (const auto& entry : std::filesystem::directory_iterator(p_input_dir)) {
        // Get file path and extension
        const auto& path = entry.path();
        std::string extension = path.extension().string();

        // Convert extension to lowercase for comparison
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

        // Check if the file has a valid image extension
        if (std::find(validExtensions.begin(), validExtensions.end(), extension) != validExtensions.end()) {
            std::cout << "Opening image: " << path.string() << std::endl;

            // Read the image using OpenCV
            cv::Mat image = cv::imread(path.string());

            if (! image.data) {
                std::cerr << "Failed to load image: " << path.string() << std::endl;
                continue;
            }

			cv::Point2i image_size = cv::Point2i(IMAGE_WIDTH, IMAGE_HEIGHT);
			cv::resize(image, image, cv::Size(image_size));

			std::chrono::time_point start = std::chrono::high_resolution_clock::now();
			float score = detect_gw_cv(image);
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

    cv::destroyAllWindows();	
}

void detect_image(std::string p_image_name)
{

	cv::Mat image; 
	image = cv::imread(p_image_name); 

	if (!image.data) { 
		std::cerr << "Failed to load image: " << p_image_name << std::endl;
		return; 
	} 
	cv::Point2i image_size = cv::Point2i(IMAGE_WIDTH, IMAGE_HEIGHT);
	cv::resize(image, image, cv::Size(image_size));

	std::chrono::time_point start = std::chrono::high_resolution_clock::now();

	uint32_t gw_detected = detect_gw_cv(image);

	std::cout << "found " << gw_detected << " give way signs" << std::endl;

	std::chrono::time_point end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> duration = end - start;

	std::cout << "detect gw took " << duration.count() << " ms" << std::endl;

	// ------------ show img 
	cv::namedWindow("Display Image", cv::WINDOW_GUI_NORMAL); 
	cv::imshow("Display Image", image); 
	cv::waitKey(0); 
}

int main(int argc, char** argv) 
{ 
	typedef cv::Point3_<uint8_t> Pixel;

	if (argc != 2) { 
		printf("usage: main_detect <Images Dir>\n"); 
		return -1; 
	} 

	// detect_image(argv[1]);
	detect_dir_images(argv[1]);
	return 0; 
}

// ----------- shwo mask
	// cv::namedWindow("Display Image", cv::WINDOW_GUI_NORMAL); 
	// // cv::imshow("Display Image", resized_img); 
	// cv::imshow("Display Image", red_pixels); 
	// cv::waitKey(0); 

	// // cv::Mat resized_img;
	// // float scale_for_print = 0.6;
	// // cv::resize(image, resized_img, cv::Size(), scale_for_print , scale_for_print);

	// cv::namedWindow("Display Image", cv::WINDOW_GUI_NORMAL); 
	// // cv::imshow("Display Image", resized_img); 
	// cv::imshow("Display Image", image); 
	// cv::waitKey(0); 

// code for checking each pixel in an image
	// int found_red = 0;
	// for (int i = 0; i < image.rows; i++)
	// {
	// 	for (int j = 0; j < image.cols; j++)
	// 	{
	// 		if(is_red(hsv_image.at<cv::Vec3b>(i, j)))
	// 		{
	// 			image.at<cv::Vec3b>(i, j) = cv::Vec3b({0, 0, 255});
	// 			found_red += 1;
	// 		} 

	// 		if(is_white(hsv_image.at<cv::Vec3b>(i, j)))
	// 		{
	// 			image.at<cv::Vec3b>(i, j) = cv::Vec3b({255, 255, 255});
	// 			found_red += 1;
	// 		}
	// 	}
	// }
	// std::cout << "found " << found_red << " red or white" << std::endl;
	// std::cout << "total pixels: " << image.cols * image.rows << std::endl;

