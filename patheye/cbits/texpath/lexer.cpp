#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "lexer.hpp"

namespace wiz {
namespace mdlpath {

lexer::lexer ()
	: state (State::START)
	, start_line (1)
	, start_character (1)
	, line (1)
	, character (1)
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
					case ',':
						on_token (Token::COMMA);
						break;
					case '(':
						on_token (Token::LEFT_PAREN);
						break;
					case ')':
						on_token (Token::RIGHT_PAREN);
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
					case 'c':
					case 'C':
						state = State::STRING_C;
						break;
					case 'o':
					case 'O':
						state = State::STRING_O;
						break;
					case 's':
					case 'S':
						state = State::STRING_S;
						break;
					case 'p':
					case 'P':
						state = State::STRING_P;
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
			case State::STRING_C:
				switch (c)
				{
					case 'l':
					case 'L':
						state = State::STRING_CL;
						break;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_CL:
				switch (c)
				{
					case 'o':
					case 'O':
						state = State::STRING_CLO;
						break;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_CLO:
				switch (c)
				{
					case 's':
					case 'S':
						state = State::STRING_CLOS;
						break;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_CLOS:
				switch (c)
				{
					case 'e':
					case 'E':
						state = State::STRING_CLOSE;
						break;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_CLOSE:
				switch (c)
				{
					case 'd':
					case 'D':
						state = State::STRING_CLOSED;
						break;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_CLOSED:
				switch (c)
				{
					case ' ':
					case '\n':
					case '\r':
					case '\t':
						on_token (Token::KW_CLOSED);
						state = State::START;
						continue;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_S:
				switch (c)
				{
					case 'e':
					case 'E':
						state = State::STRING_SE;
						break;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_SE:
				switch (c)
				{
					case 'a':
					case 'A':
						state = State::STRING_SEA;
						break;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_SEA:
				switch (c)
				{
					case 'r':
					case 'R':
						state = State::STRING_SEAR;
						break;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_SEAR:
				switch (c)
				{
					case 'c':
					case 'C':
						state = State::STRING_SEARC;
						break;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_SEARC:
				switch (c)
				{
					case 'h':
					case 'H':
						state = State::STRING_SEARCH;
						break;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_SEARCH:
				switch (c)
				{
					case ' ':
					case '\n':
					case '\r':
					case '\t':
						on_token (Token::KW_SEARCH);
						state = State::START;
						continue;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_O:
				switch (c)
				{
					case 'p':
					case 'P':
						state = State::STRING_OP;
						break;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_OP:
				switch (c)
				{
					case 'e':
					case 'E':
						state = State::STRING_OPE;
						break;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_OPE:
				switch (c)
				{
					case 'n':
					case 'N':
						state = State::STRING_OPEN;
						break;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_OPEN:
				switch (c)
				{
					case 'e':
					case 'E':
						state = State::STRING_OPENE;
						break;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_OPENE:
				switch (c)
				{
					case 'd':
					case 'D':
						state = State::STRING_OPENED;
						break;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_OPENED:
				switch (c)
				{
					case ' ':
					case '\n':
					case '\r':
					case '\t':
						on_token (Token::KW_OPENED);
						state = State::START;
						continue;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_P:
				switch (c)
				{
					case 'a':
					case 'A':
						state = State::STRING_PA;
						break;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_PA:
				switch (c)
				{
					case 't':
					case 'T':
						state = State::STRING_PAT;
						break;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_PAT:
				switch (c)
				{
					case 'h':
					case 'H':
						state = State::STRING_PATH;
						break;
					default:
						lexical_exception ();
				}
				break;
			case State::STRING_PATH:
				switch (c)
				{
					case ' ':
					case '\n':
					case '\r':
					case '\t':
						on_token (Token::KW_PATH);
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
			on_token (Token::INTEGER, build);
			break;
		case State::STRING_CLOSED:
			on_token (Token::KW_CLOSED);
			break;
		case State::STRING_SEARCH:
			on_token (Token::KW_SEARCH);
			break;
		case State::STRING_OPENED:
			on_token (Token::KW_OPENED);
			break;
		case State::STRING_PATH:
			on_token (Token::KW_PATH);
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
