/*
 * Gateway.cpp
 *
 *  Created on: Dec 15, 2014
 *      Author: kaha
 */

#include "Gateway.h"

namespace GatewayPathfinding {

Gateway::Gateway() {
	this->start = Tile();
	this->end = Tile();
}

Gateway::Gateway(Tile start, Tile end) {
	this->start = start;
	this->end = end;
}
/*
Gateway::~Gateway() {
	// TODO Auto-generated destructor stub
}
*/
Tile Gateway::getStart()
{
	return start;
}

void Gateway::setStart(Tile start)
{
	this->start = start;
}

Tile Gateway::getEnd()
{
	return end;
}

void Gateway::setEnd(Tile end)
{
	this->end = end;
}

void Gateway::writeSelfToStream(ostream *out)
{
	*out << "[";
	start.writeSelfToStream(out);
	*out << "-";
	end.writeSelfToStream(out);
	*out << "]";
}

} /* namespace GatewayHeuristic */
