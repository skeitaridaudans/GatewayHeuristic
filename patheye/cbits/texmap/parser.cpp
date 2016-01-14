#include "parser.hpp"
#include <stdexcept>
#include <string>

namespace wiz {
namespace texmap {

parser::parser ()
	: state (State::START)
{
}

void parser::on_token (Token t, std::string build) try
{
	switch (state)
	{
		case State::START:
			switch (t)
			{
				case Token::KW_TYPE:
					state = State::TYPE;
					break;
				default:
					parse_exception ();
			}
			break;
		case State::TYPE:
			switch (t)
			{
				case Token::KW_OCTILE:
					state = State::HEIGHT;
					break;
				default:
					parse_exception ();
			}
			break;
		case State::HEIGHT:
			switch (t)
			{
				case Token::KW_HEIGHT:
					state = State::HEIGHT_INTEGER;
					break;
				default:
					parse_exception ();
			}
			break;
		case State::HEIGHT_INTEGER:
			switch (t)
			{
				case Token::INTEGER:
					m.height = std::stoul (build);
					state = State::WIDTH;
					break;
				default:
					parse_exception ();
			}
			break;
		case State::WIDTH:
			switch (t)
			{
				case Token::KW_WIDTH:
					state = State::WIDTH_INTEGER;
					break;
				default:
					parse_exception ();
			}
			break;
		case State::WIDTH_INTEGER:
			switch (t)
			{
				case Token::INTEGER:
					m.width = std::stoul (build);
					state = State::MAP;
					break;
				default:
					parse_exception ();
			}
			break;
		case State::MAP:
			switch (t)
			{
				case Token::KW_MAP:
					m.allocate_data ();
					state = State::DATA;
					break;
				default:
					parse_exception ();
			}
			break;
		case State::DATA:
			switch (t)
			{
				case Token::TERRAIN_OOB:
					m.terrain_add (0, 0, 0);
					break;
				case Token::TERRAIN_PASSABLE:
					m.terrain_add (255, 255, 255);
					break;
				case Token::TERRAIN_SWAMP:
					m.terrain_add (0, 192, 0);
					break;
				case Token::TERRAIN_TREE:
					m.terrain_add (8, 255, 8);
					break;
				case Token::TERRAIN_WATER:
					m.terrain_add (8, 8, 255);
					break;
				default:
					parse_exception ();
			}
			break;
		default:
			parse_exception ();
	}
}
catch (std::out_of_range &e)
{
	std::runtime_error ("");
}

[[noreturn]] void parser::parse_exception () const
{
	throw std::runtime_error ("");
}

}
}
