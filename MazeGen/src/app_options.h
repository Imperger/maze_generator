#pragma once
#include "type.h"
#include "maze_rasterizers.h"
#include <boost/program_options.hpp>
#include <string>

class app_options
{
public:
	using seed_t = size_t;
	using ascii_render_options = maze_rasterizers::ascii_rasterizer::render_options;
	using png_render_options = maze_rasterizers::png_rasterizer::render_options;
	enum class rasterizer_type { none, png, ascii };

	app_options();

	bool process(int argc, char * argv[]);

	int64_t width() const;

	int64_t height() const;

	rasterizer_type rasterizer() const;

	seed_t seed() const;

	const std::string& out() const;

	ascii_render_options ascii_rasterizer_options() const;

	const png_render_options& png_rasterizer_options() const;
private:
	void predict_rasterizer(const std::string& filename);

	void setup_size();

	void setup_out();

	void setup_rasterizer();

	void setup_seed();

	void setup_ascii_opts();

	void setup_png_opts_cell_size();

	void setup_png_opts_wall_color();

	void setup_png_opts_pass_color();

	void setup_png_opts_enter_color();

	void setup_png_opts_exit_color();

	static seed_t gen_seed();

	static void set_if_present(char val, char& dest);

	const std::string key_help = "help";
	const std::string key_size = "size";
	const std::string key_out = "out";
	const std::string key_rasterizer = "rasterizer";
	const std::string key_seed = "seed";
	const std::string key_ascii_render_options = "ascii_render_options";
	const std::string key_png_render_cell_size = "png_render_cell_size";
	const std::string key_png_render_wall_color = "png_render_wall_color";
	const std::string key_png_render_pass_color = "png_render_pass_color";
	const std::string key_png_render_enter_color = "png_render_enter_color";
	const std::string key_png_render_exit_color = "png_render_exit_color";

	boost::program_options::options_description desc;
	boost::program_options::variables_map vm;
	dimension w;
	dimension h;
	std::string out_filename;
	rasterizer_type r = rasterizer_type::none;
	seed_t s;
	ascii_render_options ascii_opts;
	png_render_options png_opts;
};

