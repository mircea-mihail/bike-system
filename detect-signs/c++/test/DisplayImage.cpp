#include <opencv2/opencv.hpp> 
#include <stdio.h> 

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

	for (Pixel &p : cv::Mat_<Pixel>(image)) {
		p.x = 255;
		p.y = 0;
		p.z = 0;
	}

	cv::Mat resizedImg;
	resize(image, resizedImg, cv::Size(), 0.3, 0.3);

	cv::namedWindow("Display Image", cv::WINDOW_FULLSCREEN); 
	cv::imshow("Display Image", resizedImg); 
	cv::waitKey(0); 

	return 0; 
}

