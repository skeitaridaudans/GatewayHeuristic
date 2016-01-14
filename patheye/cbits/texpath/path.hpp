#ifndef _WIZ_MDLPATH_PATH_HPP_
#define _WIZ_MDLPATH_PATH_HPP_

#include <vector>

namespace wiz {
namespace mdlpath {

struct point
{
	int x;
	int y;
};

class path
{
public:
	std::vector<std::pair< point, std::vector<point>>> segments;
	std::vector<point> path;
};

}
}

#endif
