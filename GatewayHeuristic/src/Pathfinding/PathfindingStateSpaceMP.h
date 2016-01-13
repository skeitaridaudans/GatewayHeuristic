/*
 * PathfindingStateSpaceMP.h
 *
 *  Created on: Dec 3, 2014
 *      Author: kaha
 */

#ifndef PATHFINDINGSTATESPACEMP_H_
#define PATHFINDINGSTATESPACEMP_H_

#include <AStar/micropather.h>
//#include <cstring>
#include "Tile.h"
#include "TraversibleMap.h"

namespace Pathfinding {

class PathfindingStateSpaceMP : public micropather::Graph {
private:
	double parallelCost;
	double diagonalCost;

protected:
	TraversibleMap *traversibleMap;

	double *savedHeuristicForTiles;
	int *goalIdForSavedHeuristic;

	virtual double evaluatedDistance(int stateID, int goalStateID);

public:
	PathfindingStateSpaceMP(TraversibleMap *tMap);
	virtual ~PathfindingStateSpaceMP();

	double octileDistance(Tile start, Tile goal);
	double octileDistance(double deltaX, double deltaY);

	//vector<pair<int, double> > getSuccessors(int stateID);
	void AdjacentCost( void* stateID, MP_VECTOR< micropather::StateCost > *adjacent );
	float LeastCostEstimate (void* stateStart, void* stateEnd );
	double evaluate(int stateID, int goalStateID);
	bool isPossibleState(int stateID);
	void writeStateToStream(int stateID, ostream *out);
	void PrintStateInfo( void* state );

	int getStateID(int x, int y);
	Tile getState(int stateID);
};

}

#endif /* PATHFINDINGSTATESPACE_H_ */
