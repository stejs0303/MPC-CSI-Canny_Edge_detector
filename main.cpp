/*******************************************************************************************
* C++14
* Autor: Jan Stejskal
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
	Image obj("moon.png", cv::IMREAD_COLOR);
	Kernel kernel;
	kernel.setKernelSize(9);
	kernel.createMedianKernel();

	Blur(obj, kernel, Median);
	Canny(obj);
	
	cv::imshow("Nacteny obrazek", *obj.image);
	cv::imshow("Cernobily", *obj.gray_scale);
	cv::imshow("blur", *obj.blurred);

	cv::waitKey();
	return 0;
}