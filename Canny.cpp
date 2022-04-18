#include "Canny.h"

// Struct, kterı uchovává 2 int hodnoty
struct pair
{
	int x = 0;
	int y = 0;

	pair() {};
	pair(int x_, int y_)
	{
		x = x_; y = y_;
	}

	pair* operator-= (pair& right) 
	{
		this->x = -right.x;
		this->y = -right.y;
		return this;
	}
};

// Posouvá offset na další pozici v jádøe/osmi-okolí : 
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
		offset.y++;
	}
}

// Funkce implementuje Cannyho hranovı detektor s interpolací intenzity pixelu pøi tenèení hrany
void Canny(Image& img, Kernel& ker, int low_tres, int high_tres)
{
	// Objekt ukládá hodnoty intenzity G a sklonu hrany theta pro následné tenèení
	cv::Mat2f* pre_processed = new cv::Mat2f(img.blurred->rows, img.blurred->cols);

	double Gx = 0, Gy = 0, G = 0, theta = 0;
	int x, y;
	pair offset;

	// Lambda funkce pro zamezení sáhnutí mimo alokovanou pamì
	auto outOfBounds = [&](pair offset)
	{
		return x + offset.x < 0 || x + offset.x >= img.image->cols || y + offset.y < 0 || y + offset.y >= img.image->rows;
	};

	// Urèení intenzity a sklonu hrany
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

			pre_processed->at<cv::Vec2f>(y,x) = { (float)G, (float)theta };
			img.sobel->at<uchar>(y, x) = (uchar)G;
		}
	}
	
	// Tenèení hran

	// Vytvoøení kopie objektu intenzity a sklonu hrany
	cv::Mat2f* pre_processed_copy = new cv::Mat2f(img.blurred->cols, img.blurred->rows);
	pre_processed->copyTo(*pre_processed_copy);

	pair dir1p, dir2p;
	bool dir1b = false, dir2b = false;
	double alpha = 0;
	int intensity1, intensity2, intensityAvg, intensity;

	for (y = 0; y < pre_processed->rows; y++)
	{
		for (x = 0; x < pre_processed->cols; x++)
		{
			// Pokud je hodnota pixelu 0, není potøeba tenèit hranu
			if (pre_processed_copy->at<cv::Vec2f>(y, x)[0] == 0)
			{
				img.tencena->at<uchar>(y, x) = 0;
				continue;
			}
			
			theta = pre_processed_copy->at<cv::Vec2f>(y, x)[1];
			intensity = (int)pre_processed_copy->at<cv::Vec2f>(y, x)[0];
			intensity1 = 0, intensity2 = 0, intensityAvg = 0;
			dir1b = dir2b = false;

			//Nastavení offsetù podle sklonu hrany pro vıbìr dvou pixelù, ze kterıch se interpoluje hodnota intenzity pro tenèení hrany
			/*
			*  x2     x1
			*       \                   60% intenzity x1
			*	     \                  40% intenzity x2
			*	  	  x				
			*          \				
			*		    \
			*	     x1	    x2
			*/
			if (theta < 45)
			{
				dir1p.x = dir2p.x = 1;
				dir1p.y = 0; dir2p.y = -1;
				alpha = std::sin(theta * M_PI / 180) / 0.70710678;
			}
			else if (theta >= 45 && theta < 90)
			{
				dir1p.x = 0; dir2p.x = 1;
				dir1p.y = dir2p.y = -1;
				alpha = std::cos(theta * M_PI / 180) / 0.70710678;
			}
			else if (theta >= 90 && theta < 135)
			{
				dir1p.x = 0; dir2p.x = -1;
				dir1p.y = dir2p.y = -1;
				alpha = std::abs(std::cos(theta * M_PI / 180) / 0.70710678);
			}
			else
			{
				dir1p.x = dir2p.x = -1;
				dir1p.y = 0; dir2p.y = -1;
				alpha = std::abs(std::sin(theta * M_PI / 180) / 0.70710678);
			}

			if (!outOfBounds(dir1p)) intensity1 = (int)pre_processed_copy->at<cv::Vec2f>(y + dir1p.y, x + dir1p.x)[0];
			if (!outOfBounds(dir2p)) intensity2 = (int)pre_processed_copy->at<cv::Vec2f>(y + dir2p.y, x + dir2p.x)[0];

			// Pokud je intenzita pixelu menší ne intenzita interpolované hodnoty v jednom ze smìrù, pixel je potlaèen
			if (intensity < alpha * intensity2 + (1 - alpha) * intensity1)
			{
				pre_processed->at<cv::Vec2f>(y, x) = NULL;
				img.tencena->at<uchar>(y, x) = (uchar)pre_processed->at<cv::Vec2f>(y, x)[0];
				continue;
			}

			// Zmìna offsetù pro vıpoèet v opaèném smìru sklonu
			dir1p -= dir1p;
			dir2p -= dir2p;

			intensity1 = intensity2 = 0;

			if (!outOfBounds(dir1p)) intensity1 = (int)pre_processed_copy->at<cv::Vec2f>(y + dir1p.y, x + dir1p.x)[0];
			if (!outOfBounds(dir2p)) intensity2 = (int)pre_processed_copy->at<cv::Vec2f>(y + dir2p.y, x + dir2p.x)[0];

			if (intensity < alpha * intensity2 + (1 - alpha) * intensity1)
			{
				pre_processed->at<cv::Vec2f>(y, x) = NULL;
			}

			//Pokud je pixel silnìjí ne hodnoty intenzity v obou smìrech, je zachován
			img.tencena->at<uchar>(y, x) = (uchar)pre_processed->at<cv::Vec2f>(y, x)[0];
		}
	}
	delete pre_processed_copy;

	// Prahování
	for (y = 0; y < pre_processed->rows; y++)
	{
		for (x = 0; x < pre_processed->cols; x++)
		{
			img.edges->at<uchar>(y, x) = 0;

			// Pokud je intenzita silnìjší ne vysoká hranice, je povaován za hranu
			if (pre_processed->at<cv::Vec2f>(y, x)[0] >= high_tres)
			{
				img.edges->at<uchar>(y, x) = 255;
				continue;
			}
			// Pokud je menší ne nízká hranice, je potlaèen
			if (pre_processed->at<cv::Vec2f>(y, x)[0] <= low_tres) 
				continue;

			// Pokud je hodnota vyšší ne nízká hranice, ale nií ne vysoká, zkontroluje se osmi-okolí a hledá se pixel, kterı má hodnotu vyšší ne vysoká hranice
			offset.x = offset.y = -1;
			for (int k = 0; k < ker.sobel_maskx.size(); k++)
			{
				if (!outOfBounds(offset) && pre_processed->at<cv::Vec2f>(y + offset.y, x + offset.x)[0] >= high_tres)
				{
					img.edges->at<uchar>(y, x) = 255;
					break;
				}
				next(offset);
			}
		}
	}

	delete pre_processed;
}