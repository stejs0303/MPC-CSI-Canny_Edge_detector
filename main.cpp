#include <iostream>
#include <opencv2/opencv.hpp>

#include "ConvolutionKernel.h"
#include "Image.h"
#include "Blur.h"
#include "Canny.h"

/**********************************************************************************************
* Autor:		Jan Stejskal
* ID:			211272
* P�edm�t:		MPC-CSI
* 
* T�ma:			Hranov� detektor
* 
* Popis:		Hrany jsou detekov�ny pomoc� implementovan�ho Cannyho hranov�ho detektoru.
*				Ten se skl�d� ze �ty� hlavn�ch krok�:
*					1. Aplikace rozmaz�n�, z tohoto d�vodu byl implementov�n Gauss�v a 
*					   medi�nov� konvolu�n� filtr.
*					2. Aplikace Sobelova oper�toru pro v�po�et intenzity a sklonu hrany 
*					   pixelu.
*					3. Ten�en� hran.
*					4. A dvojit� prahov�n�. Pokud je hodnota intenzity pixelu vy��� ne�
*					   vysok� hranice, je pixel pova�ov�n za silnou hranu. V p��pad�, �e je 
*					   hodnota pixelu ni��� ne� n�zk� hranice, je potla�en. Pokud se nach�z� 
*					   mezi t�mito hranicemi, dojde ke zkoum�n� jeho osmi-okol�. V moment�, 
*					   kdy se poda�� nal�zt silnou hranu, je i tento pixel pova�ov�n za hranu.
*					   Pokud se v�ak v jeho osmi-okol� siln� hrana nenach�z�, je pixel potla�en.
* 
* Z�v�r:		Je vid�t, �e pou�it� medi�nov�ho filtru n�m d� lep�� v�sledek, a v�ak jeho
*				aplikace je pomalej�� ne� p�i vyu�it� Gaussova filtru.
* 
*				Nejlep�� v�sledek by jsme pravd�podobn� dostali p�i pou�it� Adaptivn�ho
*				Gaussova filtru, kter� kombinuje vlastnosti obou implementovan�ch filtr�. Ten 
*				jsem v�ak kvuli velk�mu po�tu projekt� nestihnul implementovat.
* 
* P�ipraven� sc�n��e:
*				1		-		Gauss�v konvolu�n� filtr, velikost 5x5, Sigma rovna 1.3.
*				2		-		Medi�nov� konvolu�n� filtr, velikost 9x9.
*				jin�	-		Mo�nost volby vlastn�ch hodnot.
* 
* Konfigurace:	Windows SDK 10.0.22000.0, Visual studio v143, C++14, OpenCV v455, Release x64
**********************************************************************************************/

#define SCENAR 1

// Mo�nost volby vlastn�ch hodnot + extra obrazy

#define VELIKOST_JADRA 5		// Implementovan� velikosti j�dra jsou 3x3, 5x5, 9x9.
#define TYP_FILTRU 2			// 1 - Gauss�v filtr, 2 - Medi�nov� filtr
#define VELIKOST_SIGMA 1.5		// V p��pad� Gaussova filtru je mo�n� zm�nit hodnotu rozprost�en� sigma.
								// Je vhodn� vyb�rat hodnotu z rozsahu 1 - 2.

#define NIZSI_HRANICE 80		// Ni��� hranice pro detekci hran, vhodn� volit hodnoty z rozsahu 50 - 120.
#define VYSSI_HRANICE 200		// Vy��� hranice pro detekci hran, vhodn� volit hodnoty z rozsahu 120 - 220.

#define EXTRA 0					// 1 - Zobraz� obrazy Sobelova j�dra p�ed a po ten�en� a fotografii 
								// s aplikovan�m rozmaz�n�m.

int main(char argc, char* argv)
{
	Image image("moon.png", cv::IMREAD_COLOR);
	Kernel kernel;

#if SCENAR == 1

	kernel.setKernelSize(5);
	kernel.createGaussianKernel(1.3);
	Blur(image, kernel, Gaussian);
	Canny(image, kernel, 80, 150);

#elif SCENAR == 2

	kernel.setKernelSize(9);
	kernel.createMedianKernel();
	Blur(image, kernel, Median);
	Canny(image, kernel, 100, 150);

#else

	kernel.setKernelSize(VELIKOST_JADRA);

	#if TYP_FILTRU == 1	

		kernel.createGaussianKernel(VELIKOST_SIGMA);
		Blur(image, kernel, Gaussian);

	#else

		kernel.createMedianKernel();
		Blur(image, kernel, Median);
		
	#endif

	Canny(image, kernel, NIZSI_HRANICE, VYSSI_HRANICE);

#endif

	cv::imshow("Nacteny obrazek", *image.image);

#if EXTRA == 1
	cv::imshow("blur", *image.blurred);
	cv::imshow("sobel", *image.sobel);
	cv::imshow("sobel_thin", *image.sobel_thin);
#endif

	cv::imshow("hrany", *image.edges);

	cv::waitKey();
	return 0;
}