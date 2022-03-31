#include "Canny.h"

void next(int& x_offset, int& y_offset)
{
	if (x_offset == 1)
	{
		y_offset++;
		x_offset = -1;
	}
	else
	{
		x_offset++;
	}
}

void setOffset(int num, int& x_offset, int& y_offset)
{
	switch (num)
	{
	case 0:
		x_offset = 0;
		y_offset = 1;
		break;
	case 45:
		x_offset = 1;
		y_offset = 1;
		break;
	case 90:
		x_offset = 1;
		y_offset = 0;
		break;
	case 135:
		x_offset = -1;
		y_offset = 1;
		break;
	default:
		break;
	}
}

void Canny(Image& img, Kernel& ker, int low_tres, int high_tres)
{
	cv::Mat2i* pre_processed = new cv::Mat2i(img.blurred->cols, img.blurred->rows);

	double Gx = 0, Gy = 0, G = 0, theta = 0;
	int x_offset = -1, y_offset = -1, x, y;

	auto inBounds = [&]()
	{
		return !(x + x_offset < 0 || x + x_offset >= img.image->cols || y + y_offset < 0 || y + y_offset >= img.image->rows);
	};

	// Urèení intenzity a sklonu hrany
	for (y = 0; y < img.blurred->rows; y++)
	{
		for (x = 0; x < img.blurred->cols; x++)
		{
			x_offset = y_offset = -1;
			Gx = Gy = G = theta = 0;

			for (int k = 0; k < ker.sobel_maskx.size(); k++)
			{
				if (inBounds())
				{
					Gx += ker.sobel_maskx.at(k) * (float)img.blurred->at<uchar>(x + x_offset, y + y_offset);
					Gy += ker.sobel_masky.at(k) * (float)img.blurred->at<uchar>(x + x_offset, y + y_offset);
				}
				next(x_offset, y_offset);
			}

			G = std::hypot(Gx, Gy);
			theta = std::atan2(Gy, Gx) * 180/M_PI;

			//Rozdìlení do 4 skupin 0°, 45°, 90° nebo 135°
			theta = theta < 22.5 || theta > 157.5 ? 0 : theta >= 22.5 && theta < 67.5 ? 45 : theta >= 67.5 && theta < 112.5 ? 90 : 135;

			pre_processed->at<cv::Vec2i>(y,x) = { (int)G, (int)theta };
			img.sobel->at<uchar>(y, x) = (uchar)G;
		}
	}
	
	// Tenèení hran
	// Add interpolation?
	for (y = 0; y < pre_processed->rows; y++)
	{
		for (x = 0; x < pre_processed->cols; x++)
		{
			setOffset(pre_processed->at<cv::Vec2i>(y, x)[0], x_offset, y_offset);

			bool dir1 = false, dir2 = false;

			if (inBounds()) 
				pre_processed->at<cv::Vec2i>(y, x)[0] > pre_processed->at<cv::Vec2i>(y + y_offset, x + x_offset)[0] ?
				dir1 = true : dir1 = false;

			x_offset = -x_offset;
			y_offset = -y_offset;

			if (inBounds())
				pre_processed->at<cv::Vec2i>(y, x)[0] > pre_processed->at<cv::Vec2i>(y + y_offset, x + x_offset)[0] ?
				dir2 = true : dir2 = false;

			if (!(dir1 && dir2))
				pre_processed->at<cv::Vec2i>(y,x) = NULL;

			img.tencena->at<uchar>(y, x) = (uchar)pre_processed->at<cv::Vec2i>(y, x)[0];
		}
	}

	// Prahování
	for (y = 0; y < pre_processed->rows; y++)
	{
		for (x = 0; x < pre_processed->cols; x++)
		{
			img.edges->at<uchar>(y, x) = 0;

			if (pre_processed->at<cv::Vec2i>(y, x)[0] >= high_tres)
			{
				img.edges->at<uchar>(y, x) = 255;
				continue;
			}
			if (pre_processed->at<cv::Vec2i>(y, x)[0] <= low_tres) 
				continue;

			x_offset = y_offset = -1;

			for (int k = 0; k < ker.sobel_maskx.size(); k++)
			{
				if (inBounds() && pre_processed->at<cv::Vec2i>(y + y_offset, x + x_offset)[0] >= 255)
				{
					img.edges->at<uchar>(y, x) = 255;
					break;				
				}
				next(x_offset, y_offset);
			}
		}
	}

	delete pre_processed;
}