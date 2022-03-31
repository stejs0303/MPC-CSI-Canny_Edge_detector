#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>
#include "ConvolutionKernel.h"
#include "Image.h"

void Blur(Image& img, Kernel& ker, uchar (&type)(Image&, Kernel&, int&, int&, int, int));

uchar Gaussian(Image& img, Kernel& ker, int& x, int& y, int x_offset, int y_offset);
uchar Median(Image& img, Kernel& ker, int& x, int& y, int x_offset, int y_offset);
uchar Adaptive(Image& img, Kernel& ker, int& x, int& y, int x_offset, int y_offset);