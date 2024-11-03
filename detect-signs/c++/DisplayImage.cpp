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

	if (!image.data) { 
		printf("No image data \n"); 
		return -1; 
	} 

	cv::Mat hsvImage;
	cv::cvtColor(image, hsvImage, cv::COLOR_BGR2HSV); 

	std::chrono::time_point start = std::chrono::high_resolution_clock::now();

	int found_red = 0;

	for (int i = 0; i < image.rows; i++)
	{
		for (int j = 0; j < image.cols; j++)
		{
			cv::Vec3b hsv_px = hsvImage.at<cv::Vec3b>(i, j);

			if(isRed(hsv_px[HUE], hsv_px[SATURATION], hsv_px[VALUE]))
			{
				// image.at<cv::Vec3b>(i, j) = cv::Vec3b({255, 0, 255});
				found_red += 1;
			} 
		}
	}

	std::chrono::time_point end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> duration = end - start;

	std::cout << "to iterate and change rgb took " << duration.count() << "ms" << std::endl;
	std::cout << "found " << found_red << " red" << std::endl;

	cv::Mat resizedImg;
	resize(image, resizedImg, cv::Size(), 0.3, 0.3);

	cv::namedWindow("Display Image", cv::WINDOW_FULLSCREEN); 
	cv::imshow("Display Image", resizedImg); 
	cv::waitKey(0); 

	return 0; 
}

