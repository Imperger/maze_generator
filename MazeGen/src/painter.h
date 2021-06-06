#pragma once
#include "canvas.h"

class painter
{
public:
	explicit painter(canvas& cv);

	/**
	* Draw square
	* @param pos position of left top corner
	* @param side side length
	* @param fill_color fill color
	 */
	void square(size_t x, size_t y, canvas::dimension side, canvas::color fill_color);
private:
	canvas& cv;
};

