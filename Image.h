#pragma once

#include <string>
#include <opencv2/opencv.hpp>

/*
* Tøída Image naèítá zadanou fotografii a uchovává odkazy na všechny vygenerované obrazy
*/

class Image
{
public:
	cv::Mat* image = nullptr;
	cv::Mat* gray_scale = nullptr;
	cv::Mat* blurred = nullptr;
	cv::Mat* sobel = nullptr;
	cv::Mat* sobel_thin = nullptr;
	cv::Mat* edges = nullptr;

	Image(const std::string& filename, const int flag)
	{
		image = new cv::Mat(cv::imread(filename, flag));
		
		gray_scale = new cv::Mat(image->rows, image->cols, CV_8U);
		blurred = new cv::Mat(image->rows, image->cols, CV_8U);
		sobel = new cv::Mat(image->rows, image->cols, CV_8U);
		sobel_thin = new cv::Mat(image->rows, image->cols, CV_8U);
		edges = new cv::Mat(image->rows, image->cols, CV_8U);

		cv::cvtColor(*image, *gray_scale, cv::COLOR_BGR2GRAY);
	}

	~Image()
	{
		delete image;
		delete gray_scale;
		delete blurred;
		delete sobel;
		delete edges;
		delete sobel_thin;
	}
};
