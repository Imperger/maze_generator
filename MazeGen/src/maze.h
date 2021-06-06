#pragma once
#include "type.h"
#include <cstdint>

struct maze
{
	enum class cell_type { wall, pass, enter, exit };

	virtual cell_type at(int64_t x, int64_t y) const = 0;

	virtual dimension width() const = 0;
	virtual dimension height() const = 0;

	~maze() = default;
};
