#pragma once

#define _USE_MATH_DEFINES

#include <iostream>
#include <math.h>
#include <vector>
#include <opencv2/opencv.hpp>

class Kernel
{
public:
	std::vector<double>* kernel = nullptr;
	int x_offset = 0, y_offset = 0, size = 0;

private:
	bool changed_size = false;

public:
	void setKernelSize(int dims)
	{
		int temp = size;
		switch (dims)
		{
		case 3:
			size = 9;
			x_offset = y_offset = -1;
			break;

		case 5:
			size = 25;
			x_offset = y_offset = -2;
			break;

		case 7:
			size = 49;
			x_offset = y_offset = -3;
			break;

		case 9:
			size = 81;
			x_offset = y_offset = -4;
			break;

		default:
			std::cout << "Spatne zadany rozmer kernelu." << std::endl;
			break;
		}
		changed_size = size != temp ? true : false;
	}

	void createGaussianKernel(double sigma = 1)
	{
		if (kernel != nullptr || changed_size) 
		{
			delete kernel;
			kernel = new std::vector<double>(size);
		}

		const double _2sigma2 = 2.0 * sigma * sigma;
		double radius = 0, sum = 0;
		int k_dim = (int)sqrt(size);

		for (int y = 0; y < k_dim; y++)
		{
			for (int x = 0; x < k_dim; x++)
			{
				radius = sqrt((x + x_offset) * (x + x_offset) + (y + y_offset) * (y + y_offset));
				kernel->at(y * k_dim + x) = exp(-(radius * radius) / _2sigma2) / (double)(M_PI * _2sigma2);
				sum += kernel->at(y * k_dim + x);
			}
		}

		for (int i = 0; i < size; i++)
		{
			kernel->at(i) /= sum;
		}
	}

	void createMedianKernel()
	{
		if (kernel != nullptr) delete kernel;
		kernel = new std::vector<double>(size);
	}

	~Kernel() {
		if(kernel != nullptr) delete kernel;
	}
};

