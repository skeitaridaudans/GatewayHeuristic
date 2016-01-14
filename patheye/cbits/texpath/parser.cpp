#include <cassert>
#include <stdexcept>
#include "parser.hpp"

namespace wiz {
namespace mdlpath {

parser::parser ()
	: state (State::SEARCH)
{
}

[[noreturn]] void parser::parse_exception () const
{
	throw std::runtime_error ("");
}

void parser::on_token (Token t, std::string build) try
{
	switch (state)
	{
		case State::SEARCH:
			switch (t)
			{
				case Token::KW_SEARCH:
					state = State::CLOSED_OR_PATH;
					break;
				default:
					parse_exception ();
			}
			break;
		case State::CLOSED_OR_PATH:
			switch (t)
			{
				case Token::KW_CLOSED:
					state = State::CLOSED_POINT;
					break;
				case Token::KW_PATH:
					state = State::PATH_POINT;
					break;
				default:
					parse_exception ();
			}
			break;
		case State::CLOSED_POINT:
			switch (t)
			{
				case Token::LEFT_PAREN:
					vbucket = nullptr;
					state = State::POINT_X;
					stack.push_back (State::PATH_OR_CLOSED_OR_OPENED);
					break;
				default:
					parse_exception ();
			}
			break;
		case State::PATH_OR_CLOSED_OR_OPENED:
			switch (t)
			{
				case Token::KW_PATH:
					p.segments.emplace_back (pbucket, std::vector<point> ());
					state = State::PATH_POINT;
					break;
				case Token::KW_CLOSED:
					state = State::CLOSED_POINT;
					break;
				case Token::KW_OPENED:
					p.segments.emplace_back (pbucket, std::vector<point> ());
					state = State::PATH_OR_CLOSED_OR_OPENED_POINT;
					break;
				default:
					parse_exception ();
			}
			break;
		case State::PATH_OR_CLOSED_OR_OPENED_POINT:
			switch (t)
			{
				case Token::KW_PATH:
					state = State::PATH_POINT;
					break;
				case Token::KW_CLOSED:
					state = State::CLOSED_POINT;
					break;
				case Token::LEFT_PAREN:
					vbucket = &p.segments.back ().second;
					state = State::POINT_X;
					stack.push_back (State::PATH_OR_CLOSED_OR_OPENED_POINT);
					break;
				default:
					parse_exception ();
			}
			break;
		case State::PATH_POINT:
			switch (t)
			{
				case Token::LEFT_PAREN:
					vbucket = &p.path;
					state = State::POINT_X;
					stack.push_back (State::PATH_POINT);
					break;
				default:
					parse_exception ();
			}
			break;
		case State::POINT_X:
			switch (t)
			{
				case Token::INTEGER:
					pbucket.x = stol (build);
					state = State::POINT_COMMA;
					break;
				default:
					parse_exception ();
			}
			break;
		case State::POINT_COMMA:
			switch (t)
			{
				case Token::COMMA:
					state = State::POINT_Y;
					break;
				default:
					parse_exception ();
			}
			break;
		case State::POINT_Y:
			switch (t)
			{
				case Token::INTEGER:
					pbucket.y = stol (build);
					state = State::POINT_RIGHT_PAREN;
					break;
				default:
					parse_exception ();
			}
			break;
		case State::POINT_RIGHT_PAREN:
			switch (t)
			{
				case Token::RIGHT_PAREN:
					if (vbucket)
						vbucket->emplace_back (pbucket);
					state = stack.back ();
					stack.pop_back ();
					break;
				default:
					parse_exception ();
			}
			break;
	}
}
catch (std::out_of_range &)
{
	parse_exception ();
}

}
}
