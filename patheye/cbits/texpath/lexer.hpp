#ifndef _WIZ_MDLPATH_LEXER_HPP_
#define _WIZ_MDLPATH_LEXER_HPP_

#include <string>

namespace wiz {
namespace mdlpath {

enum class Token
{
	KW_SEARCH,
	KW_CLOSED,
	KW_OPENED,
	KW_PATH,

	COMMA,
	LEFT_PAREN,
	RIGHT_PAREN,

	INTEGER,
};

class lexer
{
public:
	lexer ();

	lexer (const lexer &) = delete;
	lexer &operator = (const lexer &) = delete;

	void feed (char);
	void eof ();

protected:
	virtual void on_token (Token, std::string = "") = 0;

private:
	enum class State
	{
		START,
		INTEGER,
		STRING_C,
		STRING_CL,
		STRING_CLO,
		STRING_CLOS,
		STRING_CLOSE,
		STRING_CLOSED,
		STRING_S,
		STRING_SE,
		STRING_SEA,
		STRING_SEAR,
		STRING_SEARC,
		STRING_SEARCH,
		STRING_O,
		STRING_OP,
		STRING_OPE,
		STRING_OPEN,
		STRING_OPENE,
		STRING_OPENED,
		STRING_P,
		STRING_PA,
		STRING_PAT,
		STRING_PATH,
	} state;

	size_t start_line;
	size_t start_character;
	size_t line;
	size_t character;
	std::string build;

	[[noreturn]] void lexical_exception () const;
};

}
}

#endif
