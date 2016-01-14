#ifndef _WIZ_TEXMAP_MAP_HPP_
#define _WIZ_TEXMAP_MAP_HPP_

#include <cstdint>
#include <string>
#include <vector>

namespace wiz {
namespace texmap {

class map
{
public:
	size_t width;
	size_t height;
	std::vector<unsigned char> data;
	size_t cx;
	size_t cy;

	void allocate_data ()
	{
		data.resize ((width*4 + (4 - width*4 % 4) % 4) * height);
		cx = 0;
		cy = height - 1;
	}

	void set_data (size_t x, size_t y, uint8_t r, uint8_t g, uint8_t b)
	{
		size_t offset = width*4 + (4 - width*4 % 4) % 4;

		data[y*offset + x*4] = r;
		data[y*offset + x*4 + 1] = g;
		data[y*offset + x*4 + 2] = b;
		data[y*offset + x*4 + 3] = 255;
	}

	void terrain_add (uint8_t r, uint8_t g, uint8_t b)
	{
		set_data (cx, cy, r, g, b);

		++cx;

		if (cx >= width)
		{
			cx = 0;
			--cy;
		}
	}
};

}
}

#endif
