#pragma once
#include "../rasterizer.h"
#include "../maze.h"
#include <iostream>

namespace maze_rasterizers
{

class ascii_rasterizer: public rasterizer
{
public:
	struct render_options
	{
		render_options();
		explicit render_options(char wall, char pass, char enter, char exit);
		char wall_symbol;
		char pass_symbol;
		char enter_symbol;
		char exit_symbol;
	};

	explicit ascii_rasterizer(const render_options& options);

	void render(const maze& maze, const std::string& filename) override;
private:
	render_options options;
};

}
