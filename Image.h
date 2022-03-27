#pragma once

#include <string>
#include <opencv2/opencv.hpp>

// T��da, kter� bude sdru�ovat v�echny obr�zky do jednoho objektu
class Image
{
public:
	cv::Mat* image = nullptr;
	cv::Mat* gray_scale = nullptr;
	cv::Mat* blurred = nullptr;
	cv::Mat* edges = nullptr;

	// Na�ten� obr�zku, alokace pam�ti a p�eveden� na stupn� �edi
	Image(const std::string& filename, const int flag)
	{
		image = new cv::Mat(cv::imread(filename, flag));
		gray_scale = new cv::Mat(image->cols, image->rows, CV_8U);
		cv::cvtColor(*image, *gray_scale, cv::COLOR_BGR2GRAY);
	}

	// Destruktor
	~Image()
	{
		if (image != nullptr) delete image;
		if (gray_scale != nullptr) delete gray_scale;
		if (blurred != nullptr) delete blurred;
		if (edges != nullptr) delete edges;
	}
};
