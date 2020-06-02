#pragma once
#include "IntensityImage.h"
#include "ImageFactory.h"
#include <array>
#include "edge_detection.h"

namespace tr {
	void threshold(ed::matrix<x_source, y_source> &src, int tresh) {
		for (int i = 0; i < x_source; i++) {
			for (int j = 0; j < y_source; j++) {
				if (src[i][j] < tresh) {
					src[i][j] = 0;
				}
				else {
					src[i][j] = 255;
				}
			}

		}