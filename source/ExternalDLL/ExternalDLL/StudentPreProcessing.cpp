#include "StudentPreProcessing.h"
#include "EdgeDetection.h"
#include "Thresholding.h"
#include "ImageIO.h"
#include "ImageFactory.h"

IntensityImage * StudentPreProcessing::stepToIntensityImage(const RGBImage &image) const {
	return nullptr;
}

IntensityImage * StudentPreProcessing::stepScaleImage(const IntensityImage &image) const {
	return nullptr;
}

IntensityImage * StudentPreProcessing::stepEdgeDetection(const IntensityImage &image) const {
	//Gaussian blur kernel generator.
	const auto gaussian_kernel_creator = [](double sigma = 0.001) {
		const int W = 5;
		ed::matrix<double, W, W> kernel(W, W);
		double mean = W / 2;
		double sum = 0.0; // For getting the kernel values.
		for (int x = 0; x < W; ++x) {
			for (int y = 0; y < W; ++y) {
				kernel(y, x) = exp(-0.5 * (pow((x - mean) / sigma, 2.0) + pow((y - mean) / sigma, 2.0))) / (2 * 3.14159265358979323846264338327950288 * sigma * sigma);
				//get the kernel values.
				sum += kernel(y, x);
			}
		}
		//Normalize the kernel by the getting kernel value.
		for (int x = 0; x < W; ++x) {
			for (int y = 0; y < W; ++y) {
				kernel(y, x) /= sum;
			}
		};
		return kernel;
	};

	//gaussian kernel with a sigma.
	const auto gaussian_kernel = gaussian_kernel_creator(0.35);

	//Canny edge detection kernel.
	ed::matrix<int, 9, 9> edge_kernel({ {
	{0,0,0,1,1,1,0,0,0},
	{0,0,0,1,1,1,0,0,0},
	{0,0,0,1,1,1,0,0,0},
	{1,1,1,-4,-4,-4,1,1,1},
	{1,1,1,-4,-4,-4,1,1,1},
	{1,1,1,-4,-4,-4,1,1,1},
	{0,0,0,1,1,1,0,0,0},
	{0,0,0,1,1,1,0,0,0},
	{0,0,0,1,1,1,0,0,0}
} });

	//matrix from the IntensityImage.
	ed::matrix<int> img(image);

	img = ed::convolution<int, 5, 5, double>(img, gaussian_kernel);

	//Canny edge detection kernel for the first step
	img = ed::convolution<int>(img, edge_kernel);

	//Canny edge detection thresholding for step 2 and the last step.
	tr::basic_th<int>(img, 169);

	//convert matrix to an IntensityImage.
	return img.get_intensity_image_ptr();
}

IntensityImage * StudentPreProcessing::stepThresholding(const IntensityImage &image) const {
	IntensityImage * img_ptr = ImageFactory::newIntensityImage();
	img_ptr->set(image.getWidth(), image.getHeight());
	for (int y = 0; y < image.getHeight(); y++) {
		for (int x = 0; x < image.getWidth(); x++) {
			img_ptr->setPixel(x, y, image.getPixel(x, y));
		}
	}
	return img_ptr;
}