#pragma once
#include "../rasterizer.h"
#include "../type.h"
#include "../maze.h"
#include <boost/gil/typedefs.hpp>
#include <cstdint>

namespace maze_rasterizers
{

class png_rasterizer: public rasterizer
{
public:
	struct render_options
	{
		int64_t wall_side_length;
		color wall_color;
		color pass_color;
		color enter_color;
		color exit_color;
	};

	explicit png_rasterizer(const render_options& options);

	void render(const maze& maze, const std::string& filename) override;
private:
	render_options options;
};

}