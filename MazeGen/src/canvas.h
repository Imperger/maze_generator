#pragma once
#include <cstdint>
#include <boost/gil/typedefs.hpp>

struct canvas
{
	using dimension = int64_t;
	using color = boost::gil::rgba8_pixel_t;

	virtual void pixel(int64_t x, int64_t y, color color) = 0;

	virtual ~canvas() = default;
};