#include "app_options.h"
#include <algorithm>
#include <random>
#include <string>
#include <functional>
#include <iostream>
#include <sstream>
#include <boost/algorithm/string.hpp>

namespace po = boost::program_options;
using va = po::validation_error;
using rasterizer_type = app_options::rasterizer_type;
using seed_t = app_options::seed_t;
using ascii_render_options = app_options::ascii_render_options;
using png_render_options = app_options::png_render_options;

// WxH
struct dimensions
{
	int64_t w;
	int64_t h;
};

void validate(boost::any& v, const std::vector<std::string>& values, dimensions*, int)
{
	if (values.empty())
		throw po::validation_error(po::validation_error::kind_t::invalid_option, "Missing value");

	std::string const & d = values.front();
	auto delim = std::find(std::begin(d), std::end(d), 'x');

	if (delim == std::end(d))
		throw po::validation_error(po::validation_error::kind_t::invalid_option, "Missing dimension delimiter 'x'");

	size_t delim_idx = std::distance(std::begin(d), delim);

	dimensions ret;
	try
	{
		ret.w = std::stoll(d.substr(0, delim_idx));
		ret.h = std::stoll(d.substr(delim_idx + 1));
	}
	catch (...)
	{
		throw va(va::kind_t::invalid_option, "Failed to parse dimension value");
	}

	const int64_t min_side = 3;

	if (ret.w < min_side)
		throw va(va::kind_t::invalid_option, "Width must be greater or equal " + std::to_string(min_side));

	if (ret.h < min_side)
		throw va(va::kind_t::invalid_option, "Height must be greater or equal " + std::to_string(min_side));

	v = ret;
}

namespace maze_rasterizers
{

ascii_render_options populate_ascii_options(const std::string& value)
{
	ascii_render_options ret;

	std::stringstream ss(value);

	if (!(ss >> ret.wall_symbol))
		return ret;

	if (!(ss >> ret.pass_symbol))
		return ret;

	if (!(ss >> ret.enter_symbol))
		return ret;

	if (!(ss >> ret.exit_symbol))
		return ret;

	return ret;
}

void validate(boost::any& v, const std::vector<std::string>& values, ascii_render_options*, int)
{
	if (values.empty())
		throw po::validation_error(po::validation_error::kind_t::invalid_option, "Missing value");

	v = populate_ascii_options(values.front());
}

}

namespace boost::gil
{

color populate_color(const std::string& value)
{
	std::stringstream ss(value);

	int16_t r = 0, g = 0, b = 0, a = 255;
	if (!(ss >> r >> g >> b))
		throw va(va::kind_t::invalid_option, "Invalid color value");

	ss >> a;

	return color{ static_cast<uint8_t>(r),
		static_cast<uint8_t>(g),
		static_cast<uint8_t>(b),
		static_cast<uint8_t>(a) };
}

void validate(boost::any& v, const std::vector<std::string>& values, color*, int)
{
	if (values.empty())
		throw po::validation_error(po::validation_error::kind_t::invalid_option, "Missing value");

	v = populate_color(values.front());
}

}

namespace
{
char const* _s(const std::string& str) { return str.c_str(); }
}

app_options::app_options() : desc("Usage:") {}

bool app_options::process(int argc, char * argv[])
{
	desc.add_options()
		(_s(key_help), "Show this text")
		(_s(key_size), po::value<dimensions>()->default_value(dimensions{ 64, 64 }, "64x64"), "Maze dimensions. Format: WxH, for example 50x30")
		(_s(key_out), po::value<std::string>(), "Output filename")
		(_s(key_rasterizer), po::value<std::string>(), "Specify rasterizer. Valid values: 'ascii' or 'png'")
		(_s(key_seed), po::value<seed_t>(), "The number based on which the maze will be generated")
		(_s(key_ascii_render_options), po::value<ascii_render_options>()->default_value(ascii_render_options{ 'X', ' ', 's', 'f' }, "'X' ' ' 's' 'f'"), "List of symbols \"[wall] [pass] [enter] [exit]\"")
		(_s(key_png_render_cell_size), po::value<int64_t>()->default_value(25), "Set cell side length")
		(_s(key_png_render_wall_color), po::value<color>()->default_value(color{ 0, 0, 0, 255 }, "0 0 0 255"), "Set wall color")
		(_s(key_png_render_pass_color), po::value<color>()->default_value(color{ 200, 200, 200, 255 }, "200 200 200 255"), "Set passage color")
		(_s(key_png_render_enter_color), po::value<color>()->default_value(color{ 255, 0, 0, 255 }, "255 0 0 255"), "Set enter color")
		(_s(key_png_render_exit_color), po::value<color>()->default_value(color{ 0, 255, 0, 255 }, "0 255 0 255"), "Set exit color");

	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count(key_help))
	{
		std::cout << desc;

		return false;
	}

	setup_size();
	setup_rasterizer();
	setup_out();
	setup_seed();
	setup_ascii_opts();
	setup_png_opts_cell_size();
	setup_png_opts_wall_color();
	setup_png_opts_pass_color();
	setup_png_opts_enter_color();
	setup_png_opts_exit_color();

	return true;
}

int64_t app_options::width() const
{
	return w;
}

int64_t app_options::height() const
{
	return h;
}

rasterizer_type app_options::rasterizer() const
{
	return r;
}

seed_t app_options::seed() const
{
	return s;
}

const std::string & app_options::out() const
{
	return out_filename;
}

ascii_render_options app_options::ascii_rasterizer_options() const
{
	return ascii_opts;
}

const png_render_options & app_options::png_rasterizer_options() const
{
	return png_opts;
}

void app_options::predict_rasterizer(const std::string & filename)
{
	if (boost::algorithm::iends_with(filename, ".png"))
		r = rasterizer_type::png;
	else if (boost::algorithm::iends_with(filename, ".txt"))
		r = rasterizer_type::ascii;
	else
		r = rasterizer_type::none;
}

void app_options::setup_size()
{
	if (vm.count(key_size))
	{
		dimensions const & dim = vm[key_size].as<dimensions>();

		w = dim.w;
		h = dim.h;
	}
}

void app_options::setup_out()
{
	if (vm.count(key_out))
	{
		out_filename = vm[key_out].as<std::string>();
	}
	else
	{
		const std::string base = "maze";

		switch (r)
		{
		case rasterizer_type::ascii:
			out_filename = base + ".txt";;
			break;
		case rasterizer_type::png:
		case rasterizer_type::none:
			out_filename = base + ".png";
			break;
		}
	}

	if (r == rasterizer_type::none)
	{
		if (boost::algorithm::iends_with(out_filename, ".png"))
			r = rasterizer_type::png;
		else if (boost::algorithm::iends_with(out_filename, ".txt"))
			r = rasterizer_type::ascii;
	}
}

void app_options::setup_rasterizer()
{
	if (vm.count(key_rasterizer))
	{
		const std::string& name = vm[key_rasterizer].as<std::string>();

		if (name == "ascii")
			r = rasterizer_type::ascii;
		else if (name == "png")
			r = rasterizer_type::png;

		if (r == rasterizer_type::none)
			throw std::runtime_error("Unknown rasterizer");
	}
}

void app_options::setup_seed()
{
	s = vm.count(key_seed) ? vm[key_seed].as<seed_t>() : gen_seed();
}

void app_options::setup_ascii_opts()
{
	ascii_opts = ascii_render_options{ 'X', ' ', 's', 'f' };
	if (vm.count(key_ascii_render_options))
	{
		auto const & o = vm[key_ascii_render_options].as<ascii_render_options>();

		set_if_present(o.wall_symbol, ascii_opts.wall_symbol);
		set_if_present(o.pass_symbol, ascii_opts.pass_symbol);
		set_if_present(o.enter_symbol, ascii_opts.enter_symbol);
		set_if_present(o.exit_symbol, ascii_opts.exit_symbol);
	}
}

void app_options::setup_png_opts_cell_size()
{
	if (vm.count(key_png_render_cell_size))
	{
		png_opts.wall_side_length = vm[key_png_render_cell_size].as<int64_t>();
	}
}

void app_options::setup_png_opts_wall_color()
{
	if (vm.count(key_png_render_wall_color))
	{
		png_opts.wall_color = vm[key_png_render_wall_color].as<color>();
	}
}

void app_options::setup_png_opts_pass_color()
{
	if (vm.count(key_png_render_pass_color))
	{
		png_opts.pass_color = vm[key_png_render_pass_color].as<color>();
	}
}

void app_options::setup_png_opts_enter_color()
{
	if (vm.count(key_png_render_enter_color))
	{
		png_opts.enter_color = vm[key_png_render_enter_color].as<color>();
	}
}

void app_options::setup_png_opts_exit_color()
{
	if (vm.count(key_png_render_exit_color))
	{
		png_opts.exit_color = vm[key_png_render_exit_color].as<color>();
	}
}

seed_t app_options::gen_seed()
{
	std::random_device rd;
	std::mt19937_64 g(rd());
	std::uniform_int_distribution<int64_t> d;

	return d(g);
}

void app_options::set_if_present(char val, char & dest)
{
	if (val) dest = val;
}