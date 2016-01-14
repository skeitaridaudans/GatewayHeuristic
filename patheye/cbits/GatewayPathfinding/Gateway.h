/*
 * Gateway.h
 *
 *  Created on: Dec 15, 2014
 *      Author: kaha
 */

#ifndef GATEWAY_H_
#define GATEWAY_H_

#include <Pathfinding/Tile.h>

using namespace Pathfinding;

namespace GatewayPathfinding {

class Gateway {
public:
	Tile start;
	Tile end;

	Gateway();
	Gateway(Tile start, Tile end);
	//virtual ~Gateway();

	void writeSelfToStream(ostream *out);

	Tile getStart();
	void setStart(Tile start);
	Tile getEnd();
	void setEnd(Tile end);
};

}

#endif /* GATEWAY_H_ */
