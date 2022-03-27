#include "Blur.h"

void Blur(Image& img, Kernel& ker, uchar (&type)(Image&, Kernel&, int&, int&, int, int))
{
	// Alokace pamìti pro rozmazaný obrázek
	img.blurred = new cv::Mat(img.image->cols, img.image->rows, CV_8U);

	for (int y = 0; y < img.blurred->rows; y++)
	{
		for (int x = 0; x < img.blurred->cols; x++)
		{
			img.blurred->at<uchar>(x, y) = type(img, ker, x, y, ker.x_offset, ker.y_offset);
		}
	}
}

uchar Gaussian(Image& img, Kernel& ker, int& x, int& y, int x_offset, int y_offset) 
{
	double pixel_val = 0;
	for (int k = 0; k < ker.size; k++)
	{
		pixel_val += x + x_offset < 0 || x + x_offset >= img.image->cols || y + y_offset < 0 || y + y_offset >= img.image->rows ?
			0 : ker.used_kernel[k] * (double)img.gray_scale->at<uchar>(x + x_offset, y + y_offset);

		if (x_offset == cv::abs(ker.x_offset))
		{
			y_offset++;
			x_offset = ker.x_offset;
		}
		else
		{
			x_offset++;
		}
	}
	return (uchar)pixel_val;
}

uchar Median(Image& img, Kernel& ker, int& x, int& y, int x_offset, int y_offset)
{
	for (int k = 0; k < ker.size; k++)
	{
		ker.median_vals->at(k) = x + x_offset < 0 || x + x_offset >= img.image->cols || y + y_offset < 0 || y + y_offset >= img.image->rows ?
			0 : (int)img.gray_scale->at<uchar>(x + x_offset, y + y_offset);

		if (x_offset == cv::abs(ker.x_offset))
		{
			y_offset++;
			x_offset = ker.x_offset;
		}
		else
		{
			x_offset++;
		}
	}
	std::sort(ker.median_vals->begin(), ker.median_vals->end());
	return (uchar)((ker.median_vals->at(ker.size / 2) + ker.median_vals->at(ker.size / 2 + 1)) / 2);
}

//Old code
//........................................................................................................

void MedianBlur(Image& img, Kernel& ker)
{
	// Alokace pamìti pro rozmazaný obrázek
	img.blurred = new cv::Mat(img.image->cols, img.image->rows, CV_8U);

	std::vector<int>* values = new std::vector<int>(ker.size);

	for (int y = 0; y < img.blurred->rows; y++)
	{
		for (int x = 0; x < img.blurred->cols; x++)
		{
			int x_offset = ker.x_offset, y_offset = ker.y_offset;
			double pixel_value = .0f;

			for (int k = 0; k < ker.size; k++)
			{
				values->at(k) = (int)img.gray_scale->at<uchar>(x + x_offset, y + y_offset);

				if (x_offset == cv::abs(ker.x_offset))
				{
					y_offset++;
					x_offset = ker.x_offset;
				}
				else
				{
					x_offset++;
				}
			}
			std::sort(values->begin(), values->end());
			img.blurred->at<uchar>(x, y) = (uchar)((values->at(ker.size / 2) + values->at(ker.size / 2 + 1) / 2));
		}
	}
}

//Filtr pro odstranìní ostrých jasových pøechodù 
void GaussianBlur(Image& img, Kernel& ker)
{
	// Alokace pamìti pro rozmazaný obrázek
	img.blurred = new cv::Mat(img.image->cols, img.image->rows, CV_8U);

	for (int y = 0; y < img.blurred->rows; y++)
	{
		for (int x = 0; x < img.blurred->cols; x++)
		{
			int x_offset = ker.x_offset, y_offset = ker.y_offset;
			double pixel_val = .0f;

			for (int k = 0; k < ker.size; k++)
			{
				// Pokud se nachází mimo hranice obrázku pøiète 0, 
				// jinak vypoèítá a pøiète pøíslušnou hodnotu v závislosti na vzdálenosti od støedu jádra
				pixel_val += x + x_offset < 0 || x + x_offset >= img.image->cols || y + y_offset < 0 || y + y_offset >= img.image->rows ?
					0 : ker.used_kernel[k] * (double)img.gray_scale->at<uchar>(x + x_offset, y + y_offset);

				if (x_offset == cv::abs(ker.x_offset))
				{
					y_offset++;
					x_offset = ker.x_offset;
				}
				else
				{
					x_offset++;
				}
			}
			img.blurred->at<uchar>(x, y) = (uchar)pixel_val;
		}
	}
}