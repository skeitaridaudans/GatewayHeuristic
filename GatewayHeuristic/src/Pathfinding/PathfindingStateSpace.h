/*
 * PathfindingStateSpace.h
 *
 *  Created on: Dec 3, 2014
 *      Author: kaha
 */

#ifndef PATHFINDINGSTATESPACE_H_
#define PATHFINDINGSTATESPACE_H_

#include <AStar/StateSpace.h>
//#include <cstring>
#include "Tile.h"
#include "TraversibleMap.h"

namespace Pathfinding {

class PathfindingStateSpace: public AStar::StateSpace {
private:
	double parallelCost;
	double diagonalCost;

protected:
	TraversibleMap *traversibleMap;

	double *savedHeuristicForTiles;
	int *goalIdForSavedHeuristic;

	virtual double evaluatedDistance(int stateID, int goalStateID);

public:
	PathfindingStateSpace(TraversibleMap *tMap);
	virtual ~PathfindingStateSpace();

	double octileDistance(Tile start, Tile goal);
	double octileDistance(double deltaX, double deltaY);

	vector<pair<int, double> > getSuccessors(int stateID);
	double evaluate(int stateID, int goalStateID);
	bool isPossibleState(int stateID);
	void writeStateToStream(int stateID, ostream *out);

	int getStateID(int x, int y);
	Tile getState(int stateID);
};

}

#endif /* PATHFINDINGSTATESPACE_H_ */
