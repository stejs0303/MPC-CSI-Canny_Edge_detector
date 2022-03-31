#pragma once

#define _USE_MATH_DEFINES

#include <iostream>
#include <math.h>
#include <opencv2/opencv.hpp>
#include "Image.h"
#include "ConvolutionKernel.h"

void Canny(Image& img, Kernel& ker, int low_tres, int high_tres);