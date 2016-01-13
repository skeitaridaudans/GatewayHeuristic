/*
 * PathfindingStateSpaceMP.cpp
 *
 *  Created on: Dec 3, 2014
 *      Author: kaha
 */

#include <iostream>
#include <cmath>
#include "PathfindingStateSpaceMP.h"

namespace Pathfinding {

PathfindingStateSpaceMP::PathfindingStateSpaceMP(TraversibleMap *tMap)
{
	traversibleMap = tMap;
	parallelCost = 1.0;
	diagonalCost = sqrt(2.0);

	savedHeuristicForTiles = new double[traversibleMap->getWidth() * traversibleMap->getHeight()];
	goalIdForSavedHeuristic = new int[traversibleMap->getWidth() * traversibleMap->getHeight()];
	for(int i = 0; i < traversibleMap->getWidth() * traversibleMap->getHeight(); ++i) {
		goalIdForSavedHeuristic[i] = -1;
	}
}

PathfindingStateSpaceMP::~PathfindingStateSpaceMP()
{
	delete[] savedHeuristicForTiles;
	delete[] goalIdForSavedHeuristic;
}

//vector<pair<int, double> > PathfindingStateSpaceMP::getSuccessors(int stateID)
void PathfindingStateSpaceMP::AdjacentCost( void* stateID, MP_VECTOR< micropather::StateCost > *vec )
{
	//vector<pair<int, double> > vec;
	Tile state = this->getState((long)stateID);
	bool **map = traversibleMap->getMap();
	int x = state.getX();
	int y = state.getY();
	bool openWest	= x != 0			&& map[x-1][y];
	bool openNorth	= y != 0			&& map[x][y-1];
	bool openEast	= x < traversibleMap->getWidth()-1	&& map[x+1][y];
	bool openSouth	= y < traversibleMap->getHeight()-1	&& map[x][y+1];

	bool openNorthWest = openNorth && openWest && map[x-1][y-1];
	bool openNorthEast = openNorth && openEast && map[x+1][y-1];
	bool openSouthEast = openSouth && openEast && map[x+1][y+1];
	bool openSouthWest = openSouth && openWest && map[x-1][y+1];

	if(openNorthWest){
		micropather::StateCost stateCost;
		stateCost.state = (void*)((long)this->getStateID(x-1, y-1));
		stateCost.cost = diagonalCost;
		vec->push_back(stateCost);
	}
	if(openNorthEast){
		micropather::StateCost stateCost;
		stateCost.state = (void*)((long)this->getStateID(x+1, y-1));
		stateCost.cost = diagonalCost;
		vec->push_back(stateCost);
	}
	if(openSouthEast){
		micropather::StateCost stateCost;
		stateCost.state = (void*)((long)this->getStateID(x+1, y+1));
		stateCost.cost = diagonalCost;
		vec->push_back(stateCost);
	}
	if(openSouthWest){
		micropather::StateCost stateCost;
		stateCost.state = (void*)((long)this->getStateID(x-1, y+1));
		stateCost.cost = diagonalCost;
		vec->push_back(stateCost);
	}

	if(openWest){
		micropather::StateCost stateCost;
		stateCost.state = (void*)((long)this->getStateID(x-1, y));
		stateCost.cost = parallelCost;
		vec->push_back(stateCost);
	}
	if(openNorth){
		micropather::StateCost stateCost;
		stateCost.state = (void*)((long)this->getStateID(x, y-1));
		stateCost.cost = parallelCost;
		vec->push_back(stateCost);
	}
	if(openEast){
		micropather::StateCost stateCost;
		stateCost.state = (void*)((long)this->getStateID(x+1, y));
		stateCost.cost = parallelCost;
		vec->push_back(stateCost);
	}
	if(openSouth){
		micropather::StateCost stateCost;
		stateCost.state = (void*)((long)this->getStateID(x, y+1));
		stateCost.cost = parallelCost;
		vec->push_back(stateCost);
	}

	//return vec;
}

double PathfindingStateSpaceMP::octileDistance(Tile start, Tile goal)
{
	return octileDistance(goal.x - start.x, goal.y - start.y);
}

double PathfindingStateSpaceMP::octileDistance(double deltaX, double deltaY)
{
	if(deltaX < 0) { deltaX = -deltaX; }
	if(deltaY < 0) { deltaY = -deltaY; }
	if(deltaX > deltaY) {
		return (deltaX - deltaY) * parallelCost + deltaY * diagonalCost;
	}
	else {
		return (deltaY - deltaX) * parallelCost + deltaX * diagonalCost;
	}
}

double PathfindingStateSpaceMP::evaluatedDistance(int stateID, int goalStateID)
{
	Tile state = this->getState(stateID);
	Tile goalState = this->getState(goalStateID);
	return octileDistance(state, goalState);
}

double PathfindingStateSpaceMP::evaluate(int stateID, int goalStateID)
{
	if(goalIdForSavedHeuristic[stateID] != goalStateID) {
		savedHeuristicForTiles[stateID] = evaluatedDistance(stateID, goalStateID);
		goalIdForSavedHeuristic[stateID] = goalStateID;
	}
	return savedHeuristicForTiles[stateID];
}

float PathfindingStateSpaceMP::LeastCostEstimate (void* stateStart, void* stateEnd )
{
	return evaluate((long)stateStart, (long)stateEnd);
}

bool PathfindingStateSpaceMP::isPossibleState(int stateID)
{
	Tile state = getState(stateID);
	return traversibleMap->getMap()[state.getX()][state.getY()];
}

void PathfindingStateSpaceMP::writeStateToStream(int stateID, ostream *out)
{
	this->getState(stateID).writeSelfToStream(out);
}

void PathfindingStateSpaceMP::PrintStateInfo( void* state )
{
	writeStateToStream((long)state, &std::cout);
}

int PathfindingStateSpaceMP::getStateID(int x, int y)
{
	return x * traversibleMap->getHeight() + y;
}

Tile PathfindingStateSpaceMP::getState(int stateID)
{
	return Tile(stateID / traversibleMap->getHeight(), stateID % traversibleMap->getHeight());
}

}
