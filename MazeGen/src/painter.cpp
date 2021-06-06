#include "painter.h"

using dimension = canvas::dimension;
using color = canvas::color;

painter::painter(canvas& cv) : cv(cv) { }

void painter::square(size_t x, size_t y, dimension side, color fill_color)
{
	auto x_bound = x + side;
	auto y_bound = y + side;

	for (auto xi = x; xi < x_bound; ++xi)
	{
		for (auto yi = y; yi < y_bound; ++yi)
		{
			cv.pixel(xi, yi, fill_color);
		}
	}
}
