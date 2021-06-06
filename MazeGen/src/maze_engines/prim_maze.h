#pragma once
#include "../type.h"
#include "../maze.h"
#include <cstdint>
#include <random>
#include <vector>

namespace maze_engines
{
/**
 * Prim's maze generator.
 * Minimum valid size is 3x3
 */
class prim_maze : public maze
{
public:
	struct random
	{
		std::mt19937_64 gen;
		std::uniform_int_distribution<int64_t> dist;

		size_t operator()() { return dist(gen); }
	};

	struct virtual_border
	{
		bool top;
		bool right;
		bool bottom;
		bool left;
	};

	explicit prim_maze(dimension w, dimension h);

	void generate(int64_t seed);

	cell_type at(int64_t x, int64_t y) const override;

	dimension width() const override;

	dimension height() const override;
private:
	void remove(size_t idx, std::vector<int64_t>& from);

	void add_extra_borders();

	bool has_top_border() const;

	bool has_right_border() const;

	bool has_bottom_border() const;

	bool has_left_border() const;

	size_t find_near_pass(size_t pos) const;

	/**
	 * Get list of neighbors in cartesian coordinate system on given distance,
	 * using plain array index as point
	 * @param idx index of point
	 * @param filter returns only cells with certain type
	 * @distance distance
	 * @return list of indexes
	 */
	std::vector<size_t> neighbors(size_t idx, cell_type filter, size_t distance = 2);

	void connect(size_t first, size_t last);

	dimension w, h;
	std::vector<cell_type> maze;
	bool dirty;
	virtual_border extra_border;
};

}