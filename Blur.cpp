#include "Blur.h"

void next(Kernel& ker, int& x_offset, int& y_offset)
{
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

void Blur(Image& img, Kernel& ker, uchar (&type)(Image&, Kernel&, int&, int&, int, int))
{
	for (int y = 0; y < img.blurred->rows; y++)
	{
		for (int x = 0; x < img.blurred->cols; x++)
		{
			img.blurred->at<uchar>(y, x) = type(img, ker, x, y, ker.x_offset, ker.y_offset);
		}
	}
}

uchar Gaussian(Image& img, Kernel& ker, int& x, int& y, int x_offset, int y_offset) 
{
	double pixel_val = 0;
	for (int k = 0; k < ker.size; k++)
	{
		/*
		if (x + x_offset < 0 || x + x_offset >= img.image->cols || y + y_offset < 0 || y + y_offset >= img.image->rows)
			pixel_val += 0;
		else 
			pixel_val += ker.kernel->at(k) * (double)img.gray_scale->at<uchar>(y + y_offset, x + x_offset);
		*/
		pixel_val += x + x_offset < 0 || x + x_offset >= img.image->cols || y + y_offset < 0 || y + y_offset >= img.image->rows ?
			0 : ker.kernel->at(k) * (double)img.gray_scale->at<uchar>(y + y_offset, x + x_offset);

		next(ker, x_offset, y_offset);
	}
	return (uchar)pixel_val;
}

uchar Median(Image& img, Kernel& ker, int& x, int& y, int x_offset, int y_offset)
{
	for (int k = 0; k < ker.size; k++)
	{
		/*
		if (x + x_offset < 0 || x + x_offset >= img.image->cols || y + y_offset < 0 || y + y_offset >= img.image->rows)
			ker.kernel->at(k) = 0;
		else
			ker.kernel->at(k) = (int)img.gray_scale->at<uchar>(y + y_offset, x + x_offset);
		*/
		ker.kernel->at(k) = x + x_offset < 0 || x + x_offset >= img.image->cols || y + y_offset < 0 || y + y_offset >= img.image->rows ?
			0 : (int)img.gray_scale->at<uchar>(y + y_offset, x + x_offset);

		next(ker, x_offset, y_offset);
	}
	std::sort(ker.kernel->begin(), ker.kernel->end());
	return (uchar)((ker.kernel->at(ker.size / 2) + ker.kernel->at(ker.size / 2 + 1)) / 2);
}

uchar Adaptive(Image& img, Kernel& ker, int& x, int& y, int x_offset, int y_offset)
{
	//TODO: Implement adaptive filter
	return 0;
}