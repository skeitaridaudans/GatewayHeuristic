/*
 * GatewayPathfindingStateSpace.h
 *
 *  Created on: Dec 15, 2014
 *      Author: kaha
 */

#ifndef GATEWAYPATHFINDINGSTATESPACE_H_
#define GATEWAYPATHFINDINGSTATESPACE_H_

#include <Pathfinding/PathfindingStateSpace.h>
#include "Gateway.h"
#include "GatewayMap.h"

using namespace Pathfinding;

namespace GatewayPathfinding {

class GatewayPathfindingStateSpace: public PathfindingStateSpace {

protected:
	GatewayMap *gatewayMap;

	double shortestDistanceToGate(Tile tile, Gateway gateway);

	double evaluatedDistance(int stateID, int goalStateID);

	int lastGoalID;
	vector<double> goalGateDistances;

public:
	GatewayPathfindingStateSpace(GatewayMap *gMap);
	virtual ~GatewayPathfindingStateSpace();

};

}

#endif /* GATEWAYPATHFINDINGSTATESPACE_H_ */
