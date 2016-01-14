/*
 * GateDistanceStateSpace.h
 *
 *  Created on: Jan 22, 2015
 *      Author: kaha
 */

#ifndef GATEWAYPATHFINDING_GATEDISTANCESTATESPACE_H_
#define GATEWAYPATHFINDING_GATEDISTANCESTATESPACE_H_

#include <Pathfinding/PathfindingStateSpace.h>
#include "GatewayMap.h"

namespace GatewayPathfinding {

class GateDistanceStateSpace: public Pathfinding::PathfindingStateSpace {
private:
	GatewayMap *gatewayMap;
	//set<int> *gateIDs;

	double *gateDistances;

public:
	GateDistanceStateSpace(GatewayMap *gMap);
	GateDistanceStateSpace(GatewayMap *gMap, vector<int> *gateIDs);
	virtual ~GateDistanceStateSpace();

	double evaluate(int stateID, int goalStateID);
	bool checkPossibleGoal(int stateID, double pathLength);

	void setResultArray(double *p_gateDistances);
	//void setSearchGateIDs(set<int> *gateIDs);
};

}

#endif /* GATEWAYPATHFINDING_GATEDISTANCESTATESPACE_H_ */
