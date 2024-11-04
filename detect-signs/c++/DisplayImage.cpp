#include <opencv2/opencv.hpp> 
#include <iostream>
#include <chrono>

#include "utility.h"

int main(int argc, char** argv) 
{ 
	typedef cv::Point3_<uint8_t> Pixel;

	if (argc != 2) { 
		printf("usage: DisplayImage.out <Image_Path>\n"); 
		return -1; 
	} 

	cv::Mat image; 
	image = cv::imread(argv[1], 1); 
	cv::Point2i image_size = cv::Point2i(1200, 1600);
	cv::resize(image, image, cv::Size(image_size));

	if (!image.data) { 
		printf("No image data \n"); 
		return -1; 
	} 

	cv::Mat hsv_image;
	cv::cvtColor(image, hsv_image, cv::COLOR_BGR2HSV); 

	std::chrono::time_point start = std::chrono::high_resolution_clock::now();

	int found_red = 0;

	for (int i = 0; i < image.rows; i++)
	{
		for (int j = 0; j < image.cols; j++)
		{
			cv::Vec3b hsv_px = hsv_image.at<cv::Vec3b>(i, j);

			if(is_red(hsv_px[HUE], hsv_px[SATURATION], hsv_px[VALUE]))
			{
				image.at<cv::Vec3b>(i, j) = cv::Vec3b({0, 0, 255});
				found_red += 1;
			} 

			if(is_white(hsv_px[HUE], hsv_px[SATURATION], hsv_px[VALUE]))
			{
				image.at<cv::Vec3b>(i, j) = cv::Vec3b({255, 255, 255});
				found_red += 1;
			}
		}
	}

	std::chrono::time_point end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> duration = end - start;

	std::cout << "to iterate and change rgb took " << duration.count() << " ms" << std::endl;
	std::cout << "found " << found_red << " red" << std::endl;
	std::cout << "total pixels: " << image.cols * image.rows << std::endl;

	cv::Mat resized_img;
	float scale_for_print = 0.6;
	cv::resize(image, resized_img, cv::Size(), scale_for_print , scale_for_print);

	cv::namedWindow("Display Image", cv::WINDOW_FULLSCREEN); 
	cv::imshow("Display Image", resized_img); 
	cv::waitKey(0); 

	return 0; 
}

