/*******************************************************************************************
* C++14
* Autor: Jan Stejskal
* ID: 211272
* Pøedmìt: MPC-CSI
* Téma: Hranový detektor
*******************************************************************************************/

#include <iostream>
#include <opencv2/opencv.hpp>

#include "ConvolutionKernel.h"
#include "Image.h"
#include "Blur.h"
#include "Canny.h"

int main(char argc, char* argv) 
{
	Image img("moon.png", cv::IMREAD_COLOR);
	Kernel kernel;
	kernel.setKernelSize(9);
	kernel.createGaussianKernel(1);
	//kernel.createMedianKernel();

	Blur(img, kernel, Gaussian);
	//Blur(img, kernel, Median);
	Canny(img, kernel, 80, 150);

	cv::Mat pre_processed_copy;
	cv::GaussianBlur(*img.gray_scale, pre_processed_copy, cv::Size(5, 5), 1.5);
	cv::Canny(pre_processed_copy, pre_processed_copy, 100, 150);

	cv::imshow("Nacteny obrazek", *img.image);
	//cv::imshow("Cernobily", *img.gray_scale);
	//cv::imshow("blur", *img.blurred);
	//cv::imshow("sobel", *img.sobel);
	cv::imshow("tencena", *img.tencena);
	cv::imshow("hrany", *img.edges);

	cv::imshow("opencv", pre_processed_copy);

	cv::waitKey();
	return 0;
}