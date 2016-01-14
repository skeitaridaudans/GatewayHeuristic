/*
 * GateDistanceStateSpace.cpp
 *
 *  Created on: Jan 22, 2015
 *      Author: kaha
 */

#include <GatewayPathfinding/GateDistanceStateSpace.h>

namespace GatewayPathfinding {

GateDistanceStateSpace::GateDistanceStateSpace(GatewayMap *gMap)
: PathfindingStateSpace(gMap->getTraversibleMap())
{
	gatewayMap = gMap;
	gateDistances = 0;
	//gateIDs = 0;
}

GateDistanceStateSpace::~GateDistanceStateSpace() {
	// TODO Auto-generated destructor stub
}

double GateDistanceStateSpace::evaluate(int stateID, int goalStateID)
{
	return 0;
}

bool GateDistanceStateSpace::checkPossibleGoal(int stateID, double pathLength)
{
	Tile state = getState(stateID);
	int gateID = gatewayMap->gateIdOfTile(state);
	if(gateID != -1)
	{
		//if(gateIDs == 0) {
			if(gateDistances[gateID] == -1)
			{
				gateDistances[gateID] = pathLength;
			}
/*		}
		else {
			int i = 0;
			for(set<int>::iterator it = gateIDs->begin(); it != gateIDs->end(); ++it)
			{
				if(*it == gateID && gateDistances[i] == -1)
				{
					gateDistances[i] = pathLength;
					bool allDistancesFound = true;
					for(unsigned int j = 0; j < gateIDs->size(); ++j) {
						if(gateDistances[j] == -1) {
							allDistancesFound = false;
							break;
						}
					}
					if(allDistancesFound) {
						return true;
					}
					break;
				}
				++i;
			}
		}*/
	}
	return false;
}

void GateDistanceStateSpace::setResultArray(double *p_gateDistances)
{
	gateDistances = p_gateDistances;
}
/*
void GateDistanceStateSpace::setSearchGateIDs(set<int> *gateIDs)
{
	this->gateIDs = gateIDs;
}
*/
}
