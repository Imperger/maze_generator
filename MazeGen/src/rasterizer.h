#pragma once
#include "maze.h"
#include <string>

struct rasterizer
{
	virtual void render(const maze& maze, const std::string& filename) = 0;
};