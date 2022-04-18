#pragma once

#define _USE_MATH_DEFINES

#include <iostream>
#include <math.h>
#include <vector>
#include <opencv2/opencv.hpp>

/*
* Tøída kernel obstarává tvorbu jádra konvoluce a nastavování offsetù pro snaší provádìní konvoluce
*/

class Kernel
{
public:
	std::vector<double>* kernel = nullptr;
	int x_offset = 0, y_offset = 0, size = 0;

	std::vector<int> sobel_maskx = { -1, 0, 1, -2, 0, 2, -1, 0, 1 };
	std::vector<int> sobel_masky = { -1, -2, -1, 0, 0, 0, 1, 2, 1 };

private:
	bool changed_size = false;

public:
	// Nastaví hodnoty velikosti pro následnou alokaci pamìti a offsety
	void setKernelSize(int dims)
	{
		int prev_size = size;
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
		changed_size = size != prev_size ? true : false;
	}

	// Metoda pro tvorbu Gaussova konvoluèního jádra, pokud sigma není zadána, je rovna 1
	void createGaussianKernel(double sigma = 1)
	{
		// Pokud došlo ke zmìnì velikosti jádra, alokuje se nové místo v pamìti
		if (changed_size)
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

		// Normalizace jádra
		for (int i = 0; i < size; i++)
		{
			kernel->at(i) /= sum;
		}
	}

	//Jádro mediánového filtru nelze vytvoøit pøedem, proto se pouze alokuje pamì
	void createMedianKernel()
	{
		if (changed_size) 
		{
			delete kernel;
			kernel = new std::vector<double>(size);
		}
	}

	~Kernel() {
		if(kernel != nullptr) delete kernel;
	}
};

