#include "app_options.h"
#include "maze_engines/prim_maze.h"
#include "maze_rasterizers.h"
#include <memory>
#include <iostream>

int Main(int argc, char *argv[])
{
	using ascii_r = maze_rasterizers::ascii_rasterizer;

	try
	{
		app_options opts;

		if (!opts.process(argc, argv))
			return 0;

		maze_engines::prim_maze maze(opts.width(), opts.height());

		std::unique_ptr<rasterizer> rasterizer;
		switch (opts.rasterizer())
		{
		case app_options::rasterizer_type::ascii:
			rasterizer = std::make_unique<ascii_r>(opts.ascii_rasterizer_options());
			break;
		case app_options::rasterizer_type::png:
			rasterizer = std::make_unique<maze_rasterizers::png_rasterizer>(opts.png_rasterizer_options());
			break;
		}

		maze.generate(opts.seed());

		rasterizer->render(maze, opts.out());
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << '\n';

		return 1;
	}

	return 0;
}

#ifdef _WIN32
#include <Windows.h>
int wmain(int argc, wchar_t *argv[]) {
	SetConsoleOutputCP(CP_UTF8);

	char **u8argv = new char *[argc];
	for (int argn = 0; argn < argc; ++argn) {
		int size = WideCharToMultiByte(CP_UTF8, 0, argv[argn], -1, NULL, 0, NULL, NULL);

		u8argv[argn] = new char[size];

		WideCharToMultiByte(CP_UTF8, 0, argv[argn], -1, u8argv[argn], size, NULL, NULL);
	}

	Main(argc, u8argv);
}
#else
int main(int argc, char *argv[]) { Main(argc, argv); }
#endif