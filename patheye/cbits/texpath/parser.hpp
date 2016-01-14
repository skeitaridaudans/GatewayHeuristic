#ifndef _WIZ_MDLPATH_PARSER_HPP_
#define _WIZ_MDLPATH_PARSER_HPP_

#include "lexer.hpp"
#include "path.hpp"

namespace wiz {
namespace mdlpath {

class parser : public lexer
{
public:
	path p;
	point pbucket;
	std::vector<point> *vbucket;

	parser ();

	parser (const parser &) = delete;
	parser &operator = (const parser &) = delete;

private:
	enum class State
	{
		SEARCH,
		CLOSED_OR_PATH,
		CLOSED_POINT,
		PATH_OR_CLOSED_OR_OPENED,
		PATH_OR_CLOSED_OR_OPENED_POINT,
		PATH_POINT,

		/* point */
		POINT_X,
		POINT_COMMA,
		POINT_Y,
		POINT_RIGHT_PAREN,
	} state;

	std::vector<State> stack;

	[[noreturn]] void parse_exception () const;

	virtual void on_token (Token, std::string) override;
};

}
}

#endif
