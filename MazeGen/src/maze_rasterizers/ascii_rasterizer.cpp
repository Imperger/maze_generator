#include "ascii_rasterizer.h"
#include <fstream>

namespace maze_rasterizers
{
ascii_rasterizer::ascii_rasterizer(const render_options& options) : options(options) {}

void ascii_rasterizer::render(const maze& maze, const std::string& filename)
{
	std::ofstream out(filename, std::ios_base::binary);

	for (int64_t y = 0; y < maze.height(); ++y)
	{
		for (int64_t x = 0; x < maze.width(); ++x)
		{
			char c;
			switch (maze.at(x, y))
			{
			case maze::cell_type::pass:
				c = options.pass_symbol;
				break;
			case maze::cell_type::wall:
				c = options.wall_symbol;
				break;
			case maze::cell_type::enter:
				c = options.enter_symbol;
				break;
			case maze::cell_type::exit:
				c = options.exit_symbol;
				break;
			default:
				throw std::runtime_error("Invalid cell type");
			}

			out << c;
		}

		out << '\n';
	}
}

ascii_rasterizer::render_options::render_options() :
	wall_symbol(0), pass_symbol(0), enter_symbol(0), exit_symbol(0) {}

ascii_rasterizer::render_options::render_options(char wall, char pass, char enter, char exit) :
	wall_symbol(wall), pass_symbol(pass), enter_symbol(enter), exit_symbol(exit) {}

}