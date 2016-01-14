/*
 * PathfindingState.cpp
 *
 *  Created on: Dec 3, 2014
 *      Author: kaha
 */

#include "Tile.h"

using namespace std;

namespace Pathfinding {

Tile::Tile() {
	this->x = -1;
	this->y = -1;
}

Tile::Tile(int x, int y) {
	this->x = x;
	this->y = y;
}

int Tile::getX()
{
	return x;
}

void Tile::setX(int x)
{
	this->x = x;
}

int Tile::getY()
{
	return y;
}

void Tile::setY(int y)
{
	this->y = y;
}

void Tile::writeSelfToStream(ostream *out)
{
	*out << "(" << x << "," << y << ")";
}

}
