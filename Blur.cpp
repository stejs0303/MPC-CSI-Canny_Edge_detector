#include "Blur.h"

// Posouv� offsety na dal�� pozici v j�d�e : 
// 0 > 1 > 2 >
// 3 > 4 ...
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

// Funkce proch�z� obraz, p�ed�v� sou�adnice do p��slu�n�ho typu konvoluce a ukl�d� navr�cenou hodnotu
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

// Funkce prov�d� Gaussovu konvoluci pro jeden pixel obrazu
uchar Gaussian(Image& img, Kernel& ker, int& x, int& y, int x_offset, int y_offset) 
{
	double pixel_val = 0;
	for (int k = 0; k < ker.size; k++)
	{
		// Pokud se nenach�z� mimo obraz, p�i�te k hodnot� pixelu pixel_val = kernel[k] * hodnota pixelu[x+x_offset,y+y_offset]
		pixel_val += x + x_offset < 0 || x + x_offset >= img.image->cols || y + y_offset < 0 || y + y_offset >= img.image->rows ?
			0 : ker.kernel->at(k) * (double)img.gray_scale->at<uchar>(y + y_offset, x + x_offset);

		next(ker, x_offset, y_offset);
	}
	return (uchar)pixel_val;
}

// Funkce prov�d� medi�novou konvoluci pro jeden pixel obrazu
uchar Median(Image& img, Kernel& ker, int& x, int& y, int x_offset, int y_offset)
{
	for (int k = 0; k < ker.size; k++)
	{
		// Pokud se nenach�z� mimo obraz, ulo�� do vektoru kernelu hodnotu pixelu, jinak 0
		ker.kernel->at(k) = x + x_offset < 0 || x + x_offset >= img.image->cols || y + y_offset < 0 || y + y_offset >= img.image->rows ?
			0 : (int)img.gray_scale->at<uchar>(y + y_offset, x + x_offset);

		next(ker, x_offset, y_offset);
	}
	// Hodnoty se se��d� a vr�t� se nalezen� medi�n
	std::sort(ker.kernel->begin(), ker.kernel->end());
	return (uchar)((ker.kernel->at(ker.size / 2) + ker.kernel->at(ker.size / 2 + 1)) / 2);
}
