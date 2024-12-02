#include <opencv2/opencv.hpp> 
#include <iostream>
#include <chrono>

#include "utility.h"
#include "detect_signs.h"

int main(int argc, char** argv) 
{ 
	typedef cv::Point3_<uint8_t> Pixel;

	if (argc != 2) { 
		printf("usage: DisplayImage.out <Image_Path>\n"); 
		return -1; 
	} 

	cv::Mat image; 
	image = cv::imread(argv[1], 1); 
	cv::Point2i image_size = cv::Point2i(IMAGE_WIDTH, IMAGE_HEIGHT);
	cv::resize(image, image, cv::Size(image_size));

	if (!image.data) { 
		printf("No image data \n"); 
		return -1; 
	} 

	cv::Mat hsv_image;
	
	std::chrono::time_point start = std::chrono::high_resolution_clock::now();
    cv::Mat red_pixels = cv::Mat::zeros(image.size(), CV_8UC1);

	uint32_t gw_detected = detect_gw_cv(image, red_pixels);
	// uint32_t gw_detected = detect_gw(image);

	std::cout << "found " << gw_detected << " give way signs" << std::endl;

	std::chrono::time_point end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> duration = end - start;

	std::cout << "detect gw took " << duration.count() << " ms" << std::endl;

	// ------------ show img 
	cv::namedWindow("Display Image", cv::WINDOW_GUI_NORMAL); 
	// cv::imshow("Display Image", resized_img); 
	cv::imshow("Display Image", image); 
	cv::waitKey(0); 

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

	return 0; 
}


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

