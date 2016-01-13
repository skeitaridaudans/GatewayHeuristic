/*
 * GatewayPathfindingStateSpace.h
 *
 *  Created on: Dec 15, 2014
 *      Author: kaha
 */

#ifndef GATEWAYPATHFINDINGSTATESPACEMP_H_
#define GATEWAYPATHFINDINGSTATESPACEMP_H_

#include <Pathfinding/PathfindingStateSpaceMP.h>
#include "Gateway.h"
#include "GatewayMap.h"

using namespace Pathfinding;

namespace GatewayPathfinding {

class GatewayPathfindingStateSpaceMP: public PathfindingStateSpaceMP {

protected:
	GatewayMap *gatewayMap;

	double shortestDistanceToGate(Tile tile, Gateway gateway);

	double evaluatedDistance(int stateID, int goalStateID);

	int lastGoalID;
	vector<double> goalGateDistances;

public:
	GatewayPathfindingStateSpaceMP(GatewayMap *gMap);
	virtual ~GatewayPathfindingStateSpaceMP();

};

}

#endif /* GATEWAYPATHFINDINGSTATESPACE_H_ */
