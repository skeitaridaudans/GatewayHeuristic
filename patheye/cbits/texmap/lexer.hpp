#ifndef _WIZ_TEXMAP_LEXER_HPP_
#define _WIZ_TEXMAP_LEXER_HPP_

#include <string>

namespace wiz {
namespace texmap {

enum class Token
{
	KW_HEIGHT,
	KW_MAP,
	KW_OCTILE,
	KW_TYPE,
	KW_WIDTH,

	INTEGER,

	TERRAIN_OOB,
	TERRAIN_PASSABLE,
	TERRAIN_SWAMP,
	TERRAIN_TREE,
	TERRAIN_WATER,
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
		STRING_H,
		STRING_HE,
		STRING_HEI,
		STRING_HEIG,
		STRING_HEIGH,
		STRING_HEIGHT,
		STRING_M,
		STRING_MA,
		STRING_MAP,
		STRING_O,
		STRING_OC,
		STRING_OCT,
		STRING_OCTI,
		STRING_OCTIL,
		STRING_OCTILE,
		STRING_T,
		STRING_TY,
		STRING_TYP,
		STRING_TYPE,
		STRING_W,
		STRING_WI,
		STRING_WID,
		STRING_WIDT,
		STRING_WIDTH,
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
