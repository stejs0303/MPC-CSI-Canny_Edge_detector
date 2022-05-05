#include <iostream>
#include <opencv2/opencv.hpp>

#include "ConvolutionKernel.h"
#include "Image.h"
#include "Blur.h"
#include "Canny.h"

/**********************************************************************************************
* Autor:		Jan Stejskal
* ID:			211272
* Pøedmìt:		MPC-CSI
* 
* Téma:			Hranový detektor
* 
* Popis:		Hrany jsou detekovány pomocí implementovaného Cannyho hranového detektoru.
*				Ten se skládá ze ètyø hlavních krokù:
*					1. Aplikace rozmazání, z tohoto dùvodu byl implementován Gaussùv a 
*					   mediánový konvoluèní filtr.
*					2. Aplikace Sobelova operátoru pro výpoèet intenzity a sklonu hrany 
*					   pixelu.
*					3. Tenèení hran.
*					4. A dvojité prahování. Pokud je hodnota intenzity pixelu vyšší než
*					   vysoká hranice, je pixel považován za silnou hranu. V pøípadì, že je 
*					   hodnota pixelu nižší než nízká hranice, je potlaèen. Pokud se nachází 
*					   mezi tìmito hranicemi, dojde ke zkoumání jeho osmi-okolí. V momentì, 
*					   kdy se podaøí nalézt silnou hranu, je i tento pixel považován za hranu.
*					   Pokud se však v jeho osmi-okolí silná hrana nenachází, je pixel potlaèen.
* 
* Závìr:		Je vidìt, že použití mediánového filtru nám dá lepší výsledek, a však jeho
*				aplikace je pomalejší než pøi využití Gaussova filtru.
* 
*				Nejlepší výsledek by jsme pravdìpodobnì dostali pøi použití Adaptivního
*				Gaussova filtru, který kombinuje vlastnosti obou implementovaných filtrù. Ten 
*				jsem však kvuli velkému poètu projektù nestihnul implementovat.
* 
* Pøipravené scénáøe:
*				1		-		Gaussùv konvoluèní filtr, velikost 5x5, Sigma rovna 1.3.
*				2		-		Mediánový konvoluèní filtr, velikost 9x9.
*				jiné	-		Možnost volby vlastních hodnot.
* 
* Konfigurace:	Windows SDK 10.0.22000.0, Visual studio v143, C++14, OpenCV v455, Release x64
**********************************************************************************************/

#define SCENAR 1

// Možnost volby vlastních hodnot + extra obrazy

#define VELIKOST_JADRA 5		// Implementované velikosti jádra jsou 3x3, 5x5, 9x9.
#define TYP_FILTRU 2			// 1 - Gaussùv filtr, 2 - Mediánový filtr
#define VELIKOST_SIGMA 1.5		// V pøípadì Gaussova filtru je možné zmìnit hodnotu rozprostøení sigma.
								// Je vhodné vybírat hodnotu z rozsahu 1 - 2.

#define NIZSI_HRANICE 80		// Nižší hranice pro detekci hran, vhodné volit hodnoty z rozsahu 50 - 120.
#define VYSSI_HRANICE 200		// Vyšší hranice pro detekci hran, vhodné volit hodnoty z rozsahu 120 - 220.

#define EXTRA 0					// 1 - Zobrazí obrazy Sobelova jádra pøed a po tenèení a fotografii 
								// s aplikovaným rozmazáním.

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