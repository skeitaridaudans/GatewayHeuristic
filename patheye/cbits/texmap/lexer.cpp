#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "lexer.hpp"

namespace wiz {
namespace texmap {

lexer::lexer ()
	: state (State::START)
	, start_line (1)
	, start_character (0)
	, line (1)
	, character (0)
{
}

void lexer::feed (char c)
{
	switch (c)
	{
		case '\n':
			++line;
			character = 0;
			break;
		default:
			++character;
	}

	while (true)
	{
		switch (state)
		{
			case State::START:
				build.clear ();
				start_line = line;
				start_character = character;

				switch (c)
				{
					case ' ':
					case '\n':
					case '\r':
					case '\t':
						break;
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':
						state = State::INTEGER;
						continue;
					case '@':
					case 'O':
						on_token (Token::TERRAIN_OOB);
						break;
					case '.':
					case 'G':
						on_token (Token::TERRAIN_PASSABLE);
						break;
					case 'S':
						on_token (Token::TERRAIN_SWAMP);
						break;
					case 'T':
						on_token (Token::TERRAIN_TREE);
						break;
					case 'W':
						on_token (Token::TERRAIN_WATER);
						break;
					case 'h':
						state = State::STRING_H;
						break;
					case 'm':
						state = State::STRING_M;
						break;
					case 'o':
						state = State::STRING_O;
						break;
					case 't':
						state = State::STRING_T;
						break;
					case 'w':
						state = State::STRING_W;
						break;
					default:
						lexical_exception ();
				}
				break;
			case State::INTEGER:
				switch (c)
				{
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':
						build += c;
						break;
					default:
						on_token (Token::INTEGER, build);
						state = State::START;
						continue;
				}
				break;
			case State::STRING_H:
				switch (c)
				{
					case 'e':
						state = State::STRING_HE;
						break;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_HE:
				switch (c)
				{
					case 'i':
						state = State::STRING_HEI;
						break;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_HEI:
				switch (c)
				{
					case 'g':
						state = State::STRING_HEIG;
						break;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_HEIG:
				switch (c)
				{
					case 'h':
						state = State::STRING_HEIGH;
						break;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_HEIGH:
				switch (c)
				{
					case 't':
						state = State::STRING_HEIGHT;
						break;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_HEIGHT:
				switch (c)
				{
					case ' ':
					case '\n':
					case '\r':
					case '\t':
						on_token (Token::KW_HEIGHT);
						state = State::START;
						continue;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_M:
				switch (c)
				{
					case 'a':
						state = State::STRING_MA;
						break;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_MA:
				switch (c)
				{
					case 'p':
						state = State::STRING_MAP;
						break;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_MAP:
				switch (c)
				{
					case ' ':
					case '\n':
					case '\r':
					case '\t':
						on_token (Token::KW_MAP);
						state = State::START;
						continue;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_O:
				switch (c)
				{
					case 'c':
						state = State::STRING_OC;
						break;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_OC:
				switch (c)
				{
					case 't':
						state = State::STRING_OCT;
						break;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_OCT:
				switch (c)
				{
					case 'i':
						state = State::STRING_OCTI;
						break;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_OCTI:
				switch (c)
				{
					case 'l':
						state = State::STRING_OCTIL;
						break;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_OCTIL:
				switch (c)
				{
					case 'e':
						state = State::STRING_OCTILE;
						break;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_OCTILE:
				switch (c)
				{
					case ' ':
					case '\n':
					case '\r':
					case '\t':
						on_token (Token::KW_OCTILE);
						state = State::START;
						continue;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_T:
				switch (c)
				{
					case 'y':
						state = State::STRING_TY;
						break;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_TY:
				switch (c)
				{
					case 'p':
						state = State::STRING_TYP;
						break;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_TYP:
				switch (c)
				{
					case 'e':
						state = State::STRING_TYPE;
						break;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_TYPE:
				switch (c)
				{
					case ' ':
					case '\n':
					case '\r':
					case '\t':
						on_token (Token::KW_TYPE);
						state = State::START;
						continue;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_W:
				switch (c)
				{
					case 'i':
						state = State::STRING_WI;
						break;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_WI:
				switch (c)
				{
					case 'd':
						state = State::STRING_WID;
						break;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_WID:
				switch (c)
				{
					case 't':
						state = State::STRING_WIDT;
						break;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_WIDT:
				switch (c)
				{
					case 'h':
						state = State::STRING_WIDTH;
						break;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_WIDTH:
				switch (c)
				{
					case ' ':
					case '\n':
					case '\r':
					case '\t':
						on_token (Token::KW_WIDTH);
						state = State::START;
						continue;
					default:
						lexical_exception ();
				}
				break;
		}
		break;
	}
}

void lexer::eof ()
{
	switch (state)
	{
		case State::START:
			break;
		case State::INTEGER:
			on_token (Token::INTEGER);
			break;
		case State::STRING_HEIGHT:
			on_token (Token::KW_HEIGHT);
			break;
		case State::STRING_MAP:
			on_token (Token::KW_MAP);
			break;
		case State::STRING_OCTILE:
			on_token (Token::KW_OCTILE);
			break;
		case State::STRING_TYPE:
			on_token (Token::KW_TYPE);
			break;
		case State::STRING_WIDTH:
			on_token (Token::KW_WIDTH);
			break;
		default:
			lexical_exception ();
	}
}

[[noreturn]] void lexer::lexical_exception () const
{
	std::stringstream ss;
	ss
		<< start_line << ":" << start_character << ": "
		<< "unexpected character";

	throw std::runtime_error (ss.str ());
}

}
}
