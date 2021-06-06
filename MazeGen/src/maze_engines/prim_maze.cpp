#include "prim_maze.h"
#include <algorithm>
#include <tuple>
#include <iterator>

namespace maze_engines
{
using cell_type = prim_maze::cell_type;

prim_maze::prim_maze(dimension w, dimension h) :
	w(w),
	h(h),
	maze(w * h, cell_type::wall),
	dirty(false) {}

void prim_maze::generate(int64_t seed)
{
	if (dirty)
	{
		std::fill(std::begin(maze), std::end(maze), cell_type::wall);
		dirty = false;
	}

	random rnd{ std::mt19937_64(seed), std::uniform_int_distribution<int64_t>(0, maze.size()) };

	std::vector<int64_t> pass_candidates;

	size_t entry = 0;

	maze[entry] = cell_type::pass;

	auto walls = neighbors(entry, cell_type::wall);
	pass_candidates.insert(std::end(pass_candidates), std::begin(walls), std::end(walls));

	while (!pass_candidates.empty())
	{
		// index in pass_candidates
		size_t pass_candidate_idx = rnd() % pass_candidates.size();
		// wall index in maze
		size_t candidate_idx = pass_candidates[pass_candidate_idx];

		remove(pass_candidate_idx, pass_candidates);

		if (maze[candidate_idx] == cell_type::pass)
			continue;

		auto connect_list = neighbors(candidate_idx, cell_type::pass);

		if (connect_list.empty())
			continue;

		auto pass = connect_list[rnd() % connect_list.size()];
		connect(pass, candidate_idx);

		auto walls = neighbors(candidate_idx, cell_type::wall);
		pass_candidates.insert(std::end(pass_candidates), std::begin(walls), std::end(walls));
	}

	add_extra_borders();

	maze[find_near_pass(rnd() % w + w * !extra_border.top)] = cell_type::enter;
	maze[find_near_pass(maze.size() - rnd() % w - w * !extra_border.bottom)] = cell_type::exit;

	dirty = true;
}

cell_type prim_maze::at(int64_t x, int64_t y) const
{
	if (x >= width())
		throw std::runtime_error("x >= width()");

	if (y >= height())
		throw std::runtime_error("y >= height()");

	// top
	if (extra_border.top && y == 0)
	{
		return cell_type::wall;
	}

	// right
	if (extra_border.right && x > 0 && x % (width() - 1) == 0)
	{
		return cell_type::wall;
	}

	// bottom
	if (extra_border.bottom && y == height() - 1)
	{
		return cell_type::wall;
	}

	// left
	if (extra_border.left && x == 0)
	{
		return cell_type::wall;
	}

	x -= extra_border.left;
	y -= extra_border.top;
	return maze[x + y * w];
}

dimension prim_maze::width() const
{
	return w + extra_border.left + extra_border.right;
}

dimension prim_maze::height() const
{
	return h + extra_border.top + extra_border.bottom;
}

void prim_maze::remove(size_t idx, std::vector<int64_t>& from)
{
	std::swap(from[idx], from.back());
	from.pop_back();
}

void prim_maze::add_extra_borders()
{
	extra_border.top = !has_top_border();
	extra_border.right = !has_right_border();
	extra_border.bottom = !has_bottom_border();
	extra_border.left = !has_left_border();
}

bool prim_maze::has_top_border() const
{
	for (int64_t n = 0; n < w; ++n)
	{
		if (maze[n] != cell_type::wall)
			return false;
	}

	return true;
}

bool prim_maze::has_right_border() const
{
	for (int64_t n = 2 * w - 1; n < w * h; n += w)
	{
		if (maze[n] != cell_type::wall)
			return false;
	}

	return true;
}

bool prim_maze::has_bottom_border() const
{
	for (int64_t n = w * (h - 1); n < static_cast<int64_t>(maze.size()) - 1; ++n)
	{
		if (maze[n] != cell_type::wall)
			return false;
	}

	return true;
}

bool prim_maze::has_left_border() const
{
	for (int64_t n = 0; n < w * (h - 1); n += w)
	{
		if (maze[n] != cell_type::wall)
			return false;
	}

	return true;
}

size_t prim_maze::find_near_pass(size_t pos) const
{
	auto found = std::find(std::next(std::begin(maze), pos), std::end(maze), cell_type::pass);
	if (found != std::end(maze))
		return std::distance(std::begin(maze), found);

	auto rfound = std::find(std::next(std::rbegin(maze), maze.size() - pos), std::rend(maze), cell_type::pass);
	if (rfound != std::rend(maze))
		return std::distance(std::begin(maze), rfound.base()) - 1;

	return 0;
}

std::vector<size_t> prim_maze::neighbors(size_t idx, cell_type filter, size_t distance)
{
	std::vector<size_t> ret;
	int64_t row = idx / w;

	if (int64_t neighbor = (idx - distance); neighbor >= 0 &&
		row == neighbor / w &&
		maze[neighbor] == filter)
		ret.push_back(neighbor);

	if (int64_t neighbor = idx + distance; row == neighbor / w &&
		neighbor < static_cast<int64_t>(maze.size()) &&
		maze[neighbor] == filter)
		ret.push_back(neighbor);

	if (int64_t neighbor = idx + distance * w; neighbor < static_cast<int64_t>(maze.size()) &&
		maze[neighbor] == filter)
		ret.push_back(neighbor);

	if (int64_t neighbor = idx - distance * w; neighbor >= 0 &&
		maze[neighbor] == filter)
		ret.push_back(neighbor);

	return ret;
}

void prim_maze::connect(size_t first, size_t last)
{
	int64_t step = first < last ? 1 : -1;
	if (first / w != last / w)
		step *= w;

	while (first != last)
	{
		maze[first] = cell_type::pass;

		first += step;
	}

	maze[last] = cell_type::pass;
}

}
