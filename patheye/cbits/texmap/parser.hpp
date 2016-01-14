#ifndef _WIZ_TEXMAP_PARSER_HPP_
#define _WIZ_TEXMAP_PARSER_HPP_

#include "lexer.hpp"
#include "map.hpp"

namespace wiz {
namespace texmap {

class parser : public lexer
{
public:
	map m;

	parser ();

	parser (const parser &) = delete;
	parser &operator = (const parser &) = delete;

private:
	enum class State
	{
		START,
		TYPE,
		HEIGHT,
		HEIGHT_INTEGER,
		WIDTH,
		WIDTH_INTEGER,
		MAP,
		DATA,
	} state;

	[[noreturn]] void parse_exception () const;

	virtual void on_token (Token, std::string) override;
};

}
}

#endif
