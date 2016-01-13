/*
 * PathfindingState.h
 *
 *  Created on: Dec 3, 2014
 *      Author: kaha
 */

#ifndef TILE_H_
#define TILE_H_

#include <ostream>

using namespace std;

namespace Pathfinding {

class Tile {
public:
	int x;
	int y;

public:
	Tile();
	Tile(int x, int y);
	virtual ~Tile();

	void writeSelfToStream(ostream *out);

	int getX();
	void setX(int x);
	int getY();
	void setY(int y);
};

}

#endif /* TILE_H_ */
