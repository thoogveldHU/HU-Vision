#pragma once

#include "IntensityImage.h"
#include "ImageFactory.h"
#include <array>
#include <iostream>
#include <map>

namespace ed
{
	template <class T, int Height = 0, int Width = 0>
	class matrix
	{
	public:
		int width = -1;
		int height = -1;
		T *var_matrix;

		constexpr matrix(const int height, const int width) : width(width),
															  height(height)
		{
			var_matrix = new T[height * width];
		}

		matrix(const IntensityImage &image) : width(image.getWidth()),
											  height(image.getHeight())
		{
			var_matrix = new T[height * width];
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					var_matrix[(y * width) + x] = image.getPixel(x, y);
				}
			}
		}

		template <typename TT = T>
		matrix(const std::array<std::array<TT, Width>, Height> &matrix) : width(Width),
																		  height(Height)
		{
			var_matrix = new T[height * width];
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					var_matrix[(y * width) + x] = matrix[y][x];
				}
			}
		}

		IntensityImage *get_intensity_image_ptr()
		{
			IntensityImage *img_ptr = ImageFactory::newIntensityImage();
			img_ptr->set(width, height);
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					img_ptr->setPixel(x, y, this->operator()(y, x));
				}
			}
			return img_ptr;
		}

		template <typename NT = unsigned int>
		void equalization(int spread_size)
		{
			static std::map<int, unsigned int> cdf = cdf_map();
			static std::map<unsigned int, NT> equalized_value_map;
			int lowest_non_neg_cdf_key = 0;
			int lowest_non_neg_cdf_value = 0;

			std::map<int, unsigned int>::iterator it;
			for (int i = 0;; i++)
			{
				if (cdf[i])
				{
					it = cdf.find(i);
					lowest_non_neg_cdf_value = cdf[i];
					lowest_non_neg_cdf_key = i;
					break;
				}
			}

			double MxN = width * height;
			double alpha = spread_size / (MxN - lowest_non_neg_cdf_value);

			equalized_value_map[0] = static_cast<unsigned int>(alpha * (it->second - lowest_non_neg_cdf_value));
			for (int i = 1; i < spread_size; i++)
			{
				equalized_value_map[i] = static_cast<unsigned int>(equalized_value_map[i - 1] + (alpha * (it->second - lowest_non_neg_cdf_value)));
				it = std::next(it, 1);
			}
			for (int i = 0; i < MxN; i++)
			{
				(var_matrix[i] < 0) ? (var_matrix[i] = 0) : (var_matrix[i] = equalized_value_map[var_matrix[i]]);
			}
		}

		template <unsigned MinValue = 0, unsigned MaxValue = 255>
		void thresholding_static()
		{
			for (int i = 0; i < height; i++)
			{
				for (int ii = 0; ii < width; ii++)
				{
					if (this->operator()(i, ii) <= 155 || this->operator()(i, ii) > 2500)
					{
						this->operator()(i, ii) = MaxValue;
					}
					else
					{
						this->operator()(i, ii) = MinValue;
					}
				}
			}
		}

		T &operator()(const int y, const int x)
		{
			return var_matrix[(y * width) + x];
		}

		T operator()(const int y, const int x) const
		{
			return var_matrix[(y * width) + x];
		}

		T &operator()(int n)
		{
			return var_matrix[n];
		}

		T operator()(int n) const
		{
			return var_matrix[n];
		}

	protected:
		std::map<int, unsigned int> cdf_map()
		{
			std::map<int, unsigned int> map;
			for (int i = 0; i < (width * height); i++)
			{
				if (var_matrix[i] < 0)
				{
					map[0] += 1;
				}
				else
				{
					map[var_matrix[i]] += 1;
				}
			}

			for (auto ptr = std::next(map.begin(), 1); ptr != map.end(); ptr++)
			{
				ptr->second += (std::next(ptr, -1))->second;
			}
			return map;
		}
	};
	template <typename T, int Height, int Width, typename TT = T>
	matrix<T> convolution(matrix<T> &image, const matrix<TT, Height, Width> &kernel)
	{
		unsigned int kernel_width = kernel.width;
		unsigned int kernel_height = kernel.height;

		int kernel_center_X = kernel_width / 2 + 1;
		int kernel_center_Y = kernel_height / 2 + 1;

		matrix<T> new_image(image.height, image.width);

		for (int y = 0; y < image.height; ++y)
		{
			for (int x = 0; x < image.width; ++x)
			{
				for (int yy = kernel_height - 1; yy >= 0; --yy)
				{
					for (int xx = kernel_width - 1; xx >= 0; --xx)
					{
						int image_index_Y = y + (kernel_center_Y - yy);
						int image_index_X = x + (kernel_center_X - xx);
						if (image_index_Y >= 0 && image_index_Y <= image.height && image_index_X >= 0 && image_index_X <= image.width)
						{
							new_image(y, x) += image(image_index_Y, image_index_X) * kernel(yy, xx);
						}
					}
				}
			}
		}
		return new_image;
	}
} // namespace ed