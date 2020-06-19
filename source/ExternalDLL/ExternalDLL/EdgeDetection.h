#pragma once

#include "IntensityImage.h"
#include "ImageFactory.h"
#include <array>
#include <iostream>
#include <map>

namespace ed {

	template <class T, int H = 0, int W = 0>
	class matrix {
	public:
		int width = -1;
		int height = -1;

		T* m;

		matrix(const int h, const int w) :
			width(w),
			height(h)
		{
			m = new T[height * width];
		}

		matrix(const IntensityImage& image) :
			width(image.getWidth()),
			height(image.getHeight())
		{
			m = new T[height * width];
			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					m[(y * width) + x] = image.getPixel(x, y);
				}
			}
		}

		template <typename TT = T>
		matrix(const std::array<std::array<TT, W>, H> & matrix) :
			width(W),
			height(H)
		{
			m = new T[height*width];
			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					m[(y*width) + x] = matrix[y][x];
				}
			}
		}

		IntensityImage * get_intensity_image_ptr() {
			IntensityImage * img_ptr = ImageFactory::newIntensityImage();
			img_ptr->set(width, height);
			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					img_ptr->setPixel(x, y, this->operator()(y, x));
				}
			}
			return img_ptr;
		}

		template <typename NT = unsigned int>
		void equalization(int spread_size) {
			static std::map<unsigned int, unsigned int> cdf = cdf_map();
			static std::map<unsigned int, NT> equalized_value_map;
			auto cdf_min = cdf.begin()->second;
			auto MxN = width * height;
			for (auto& pair : cdf) {
				equalized_value_map[pair.first] = (((pair.second - cdf_min) / (MxN - cdf_min)) * (spread_size - 1));
			}
		}

		T & operator()(const int y, const int x) {
			return m[(y*width) + x];
		}

		T & operator()(int n) {
			return m[n];
		}

	protected:

		std::map<unsigned int, unsigned int> cdf_map() {
			std::map<unsigned int, unsigned int> map;
			for (int i = 0; i < (width*height); i++) {
				if (m[i] < 0) {
					map[0] += 1;
				}
				else {
					map[m[i]] += 1;
				}
			}
			for (auto ptr = std::next(map.begin(), 1); ptr != map.end(); ptr++) {
				ptr->second += (std::next(ptr, -1))->second;
			}
			return map;
		}
	};


	template <typename T, T H, T W, typename TT = T>
	matrix<T> convolution(matrix<T> & image, matrix<TT, H, W> & kernel) {
		// find center position of kernel
		unsigned int kernel_width = kernel.width;
		unsigned int kernel_height = kernel.height;

		int kernel_center_X = kernel_width / 2 + 1;
		int kernel_center_Y = kernel_height / 2 + 1;

		matrix<T> new_image(image.height, image.width);

		for (int y = 0; y < image.height; ++y) {
			for (int x = 0; x < image.width; ++x) {
				for (int yy = kernel_height - 1; yy >= 0; --yy) {
					for (int xx = kernel_width - 1; xx >= 0; --xx) {

						// index of input signal
						int image_index_Y = y + (kernel_center_Y - yy);
						int image_index_X = x + (kernel_center_X - xx);

						// ignore out of bounds
						if (image_index_Y >= 0 && image_index_Y <= image.height && image_index_X >= 0 && image_index_X <= image.width) {
							new_image(y, x) += image(image_index_Y, image_index_X) * kernel(yy, xx);
						}
					}
				}
			}
		}
		return new_image;
	}
}
