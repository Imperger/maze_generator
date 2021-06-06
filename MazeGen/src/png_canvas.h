#pragma once
#include "canvas.h"

#include <boost/gil.hpp>
#include <boost/gil/extension/io/png.hpp>

class png_canvas: public canvas
{
public:
	using image = boost::gil::rgba8_image_t;

	explicit png_canvas(dimension w, dimension h, color background = { 0, 0, 0, 0 });

	void pixel(int64_t x, int64_t y, color color) override;

	void save(const std::string& filename);
private:
	dimension w, h;
	image img;
};
