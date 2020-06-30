#pragma once

#include "IntensityImage.h"
#include "ImageFactory.h"
#include "EdgeDetection.h"
namespace tr {
	template <class T>
	void basic_th(ed::matrix<T> &src, const int &th_min = 170, const int &th_max = 2500) {
		for (int i = 0; i < src.height; i++) {
			for (int j = 0; j < src.width; j++) {
				if (src(i, j) <= th_min || src(i, j) >= th_max) {
					src(i, j) = 255;
				}
				else {
					src(i, j) = 0;
				}
			}
		}
	}
}