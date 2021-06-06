#include "png_canvas.h"

namespace gil = boost::gil;
using dimension = canvas::dimension;
using color = canvas::color;

png_canvas::png_canvas(dimension w, dimension h, color background) : w(w), h(h), img(w, h)
{
	gil::fill_pixels(view(img), background);
}

void png_canvas::pixel(int64_t x, int64_t y, color color)
{
	auto& v = gil::view(img);

	*v.xy_at(x, y) = color;
}

void png_canvas::save(const std::string& filename)
{
	gil::write_view(filename, gil::view(img), gil::png_tag{});
}