#include "png_rasterizer.h"
#include "../png_canvas.h"
#include "../painter.h"
#include <array>

namespace maze_rasterizers
{

png_rasterizer::png_rasterizer(const render_options & options) : options(options) {}

void png_rasterizer::render(const maze& maze, const std::string & filename)
{
	png_canvas cv(maze.width() * options.wall_side_length, maze.height() * options.wall_side_length);
	painter p(cv);

	for (int64_t y = 0; y < maze.height(); ++y)
	{
		for (int64_t x = 0; x < maze.width(); ++x)
		{
			color c;
			switch (maze.at(x, y))
			{
			case maze::cell_type::pass:
				c = options.pass_color;
				break;
			case maze::cell_type::wall:
				c = options.wall_color;
				break;
			case maze::cell_type::enter:
				c = options.enter_color;
				break;
			case maze::cell_type::exit:
				c = options.exit_color;
				break;
			}

			p.square(x * options.wall_side_length,
				y * options.wall_side_length,
				options.wall_side_length, c);
		}
	}

	cv.save(filename);
}

}