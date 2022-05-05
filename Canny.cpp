#include "Canny.h"

// Struct, kter� uchov�v� 2 int hodnoty
struct pair
{
	int x = 0;
	int y = 0;

	pair() {};
	pair(int x_, int y_) : x(x), y(y) {}
	
	pair* operator-= (pair& right) 
	{
		this->x = -right.x;
		this->y = -right.y;
		return this;
	}
};

// Posouv� offset na dal�� pozici v j�d�e/osmi-okol� : 
// 0 > 1 > 2 >
// 3 > 4 ...
void next(pair& offset)
{
	if (offset.x == 1)
	{
		offset.y++;
		offset.x = -1;
	}
	else
	{
		offset.x++;
	}
}

// Funkce implementuje Cannyho hranov� detektor s interpolac� intenzity pixelu p�i ten�en� hrany
void Canny(Image& img, Kernel& ker, int low_tres, int high_tres)
{
	// Objekt ukl�d� hodnoty intenzity G a sklonu hrany theta pro n�sledn� ten�en�
	cv::Mat2f* intensity_and_slope = new cv::Mat2f(img.blurred->rows, img.blurred->cols);

	double Gx = 0, Gy = 0, G = 0, theta = 0;
	int x, y;
	pair offset;

	// Lambda funkce pro zamezen� s�hnut� mimo alokovanou pam�
	auto outOfBounds = [&](pair offset)
	{
		return x + offset.x < 0 || x + offset.x >= img.image->cols || y + offset.y < 0 || y + offset.y >= img.image->rows;
	};

	// Ur�en� intenzity a sklonu hrany
	for (y = 0; y < img.blurred->rows; y++)
	{
		for (x = 0; x < img.blurred->cols; x++)
		{
			offset.x = offset.y = -1;
			Gx = Gy = G = theta = 0;

			for (int k = 0; k < ker.sobel_maskx.size(); k++)
			{
				if (!outOfBounds(offset))
				{
					Gx += ker.sobel_maskx.at(k) * (float)img.blurred->at<uchar>(y + offset.y, x + offset.x);
					Gy += ker.sobel_masky.at(k) * (float)img.blurred->at<uchar>(y + offset.y, x + offset.x);
				}
				next(offset);
			}

			G = std::hypot(Gx, Gy);
			theta = std::atan2(Gy, Gx) * 180/M_PI;
			theta += theta < 0 ? 180 : 0;

			intensity_and_slope->at<cv::Vec2f>(y,x) = { (float)G, (float)theta };
			img.sobel->at<uchar>(y, x) = (uchar)G;
		}
	}
	
	// Ten�en� hran

	// Vytvo�en� kopie objektu intenzity a sklonu hrany
	// Doch�z� k p�episov�n� hodnot pixel� origin�lu, co� by mohlo ovlivnit hodnoty n�sleduj�c�ch pixel�
	cv::Mat2f* intensity_and_slope_cpy = new cv::Mat2f(img.blurred->cols, img.blurred->rows);
	intensity_and_slope->copyTo(*intensity_and_slope_cpy);

	pair direction1, direction2;
	double alpha = 0;
	int intensity, intensity1, intensity2;

	for (y = 0; y < intensity_and_slope->rows; y++)
	{
		for (x = 0; x < intensity_and_slope->cols; x++)
		{
			// Pokud je hodnota pixelu 0, nen� pot�eba ten�it hranu
			if (intensity_and_slope_cpy->at<cv::Vec2f>(y, x)[0] == 0)
			{
				img.sobel_thin->at<uchar>(y, x) = 0;
				continue;
			}
			
			theta = ((int)intensity_and_slope_cpy->at<cv::Vec2f>(y, x)[1] + 90) % 180;
			intensity = (int)intensity_and_slope_cpy->at<cv::Vec2f>(y, x)[0];
			intensity1 = 0, intensity2 = 0;

			/***************************************************************************************
			* Nastaven� offset� podle sklonu hrany pro v�b�r dvou pixel�, ze kter�ch se interpoluje 
			* hodnota intenzity pro ten�en� hrany
			* 
			*				  i2			Intenzita se vypo��t� podle vzorce: 
			*		   \					x1 = alpha * i2 + (1 - alpha) * i1,
			*			\	  x1			kde alpha je vzd�lenost x1 od i1
			*	 i1		x\    i1
			*	 x2		  \
			*			   \
			*	 i2	  
			****************************************************************************************/
			if (theta < 45)
			{
				direction1.x = direction2.x = 1;
				direction1.y = 0; direction2.y = -1;
				alpha = std::tan(theta * M_PI / 180);
			}
			else if (theta >= 45 && theta < 90)
			{
				direction1.x = 0; direction2.x = 1;
				direction1.y = direction2.y = -1;
				alpha = std::tan((90 - theta) * M_PI / 180);
			}
			else if (theta >= 90 && theta < 135)
			{
				direction1.x = 0; direction2.x = -1;
				direction1.y = direction2.y = -1;
				alpha = std::abs(std::tan((theta - 90) * M_PI / 180));
			}
			else
			{
				direction1.x = direction2.x = -1;
				direction1.y = 0; direction2.y = -1;
				alpha = std::abs(std::tan(theta * M_PI / 180));
			}

			if (!outOfBounds(direction1)) intensity1 = 
				(int)intensity_and_slope_cpy->at<cv::Vec2f>(y + direction1.y, x + direction1.x)[0];

			if (!outOfBounds(direction2)) intensity2 = 
				(int)intensity_and_slope_cpy->at<cv::Vec2f>(y + direction2.y, x + direction2.x)[0];

			// Pokud je intenzita pixelu men�� ne� intenzita interpolovan� hodnoty v jednom ze sm�r�, pixel je potla�en
			if (intensity < alpha * intensity2 + (1 - alpha) * intensity1)
			{
				intensity_and_slope->at<cv::Vec2f>(y, x) = NULL;
				img.sobel_thin->at<uchar>(y, x) = (uchar)intensity_and_slope->at<cv::Vec2f>(y, x)[0];
				continue;
			}

			// Zm�na offset� pro v�po�et v opa�n�m sm�ru sklonu
			direction1 -= direction1;
			direction2 -= direction2;

			intensity1 = intensity2 = 0;

			if (!outOfBounds(direction1)) intensity1 = 
				(int)intensity_and_slope_cpy->at<cv::Vec2f>(y + direction1.y, x + direction1.x)[0];

			if (!outOfBounds(direction2)) intensity2 = 
				(int)intensity_and_slope_cpy->at<cv::Vec2f>(y + direction2.y, x + direction2.x)[0];

			if (intensity < alpha * intensity2 + (1 - alpha) * intensity1)
			{
				intensity_and_slope->at<cv::Vec2f>(y, x) = NULL;
			}

			//Pokud je pixel siln�j� ne� hodnoty intenzity v obou sm�rech, je zachov�n
			img.sobel_thin->at<uchar>(y, x) = (uchar)intensity_and_slope->at<cv::Vec2f>(y, x)[0];
		}
	}
	delete intensity_and_slope_cpy;

	// Prahov�n�
	for (y = 0; y < intensity_and_slope->rows; y++)
	{
		for (x = 0; x < intensity_and_slope->cols; x++)
		{
			img.edges->at<uchar>(y, x) = 0;

			// Pokud je intenzita siln�j�� ne� vysok� hranice, je pova�ov�n za hranu
			if (intensity_and_slope->at<cv::Vec2f>(y, x)[0] >= high_tres)
			{
				img.edges->at<uchar>(y, x) = 255;
				continue;
			}
			// Pokud je men�� ne� n�zk� hranice, je potla�en
			if (intensity_and_slope->at<cv::Vec2f>(y, x)[0] <= low_tres) 
				continue;

			// Pokud je hodnota vy��� ne� n�zk� hranice, ale ni��� ne� vysok�, zkontroluje se osmi-okol� a hled� se pixel, kter� m� hodnotu vy��� ne� vysok� hranice
			offset.x = offset.y = -1;
			for (int k = 0; k < ker.sobel_maskx.size(); k++)
			{
				if (!outOfBounds(offset) && intensity_and_slope->at<cv::Vec2f>(y + offset.y, x + offset.x)[0] >= high_tres)
				{
					img.edges->at<uchar>(y, x) = 255;
					break;
				}
				next(offset);
			}
		}
	}

	delete intensity_and_slope;
}