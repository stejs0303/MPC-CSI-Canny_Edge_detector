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
	//Image img("moon.png", cv::IMREAD_COLOR);
	Image img("explosion.jpg", cv::IMREAD_COLOR);
	Kernel kernel;
	kernel.setKernelSize(9);
	kernel.createGaussianKernel();
	//kernel.createMedianKernel();

	Blur(img, kernel, Gaussian);
	//Blur(img, kernel, Median);
	Canny(img, kernel, 75, 150);

	cv::imshow("Nacteny obrazek", *img.image);
	cv::imshow("Cernobily", *img.gray_scale);
	cv::imshow("blur", *img.blurred);
	cv::imshow("sobel", *img.sobel);
	cv::imshow("edges", *img.edges);
	cv::imshow("tencena", *img.tencena);

	cv::waitKey();
	return 0;
}