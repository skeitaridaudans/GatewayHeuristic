/*
 * PathfindingStateSpace.cpp
 *
 *  Created on: Dec 3, 2014
 *      Author: kaha
 */

#include <cmath>
#include "PathfindingStateSpace.h"

namespace Pathfinding {

PathfindingStateSpace::PathfindingStateSpace(TraversibleMap *tMap)
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

PathfindingStateSpace::~PathfindingStateSpace()
{
	delete[] savedHeuristicForTiles;
	delete[] goalIdForSavedHeuristic;
}

vector<pair<int, double> > PathfindingStateSpace::getSuccessors(int stateID)
{
	vector<pair<int, double> > vec;
	Tile state = this->getState(stateID);
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
		vec.push_back(pair<int,double>(this->getStateID(x-1, y-1), diagonalCost)); }
	if(openNorthEast){
		vec.push_back(pair<int,double>(this->getStateID(x+1, y-1), diagonalCost)); }
	if(openSouthEast){
		vec.push_back(pair<int,double>(this->getStateID(x+1, y+1), diagonalCost)); }
	if(openSouthWest){
		vec.push_back(pair<int,double>(this->getStateID(x-1, y+1), diagonalCost)); }

	if(openWest){
		vec.push_back(pair<int,double>(this->getStateID(x-1, y), parallelCost)); }
	if(openNorth){
		vec.push_back(pair<int,double>(this->getStateID(x, y-1), parallelCost)); }
	if(openEast){
		vec.push_back(pair<int,double>(this->getStateID(x+1, y), parallelCost)); }
	if(openSouth){
		vec.push_back(pair<int,double>(this->getStateID(x, y+1), parallelCost)); }

	return vec;
}

double PathfindingStateSpace::octileDistance(Tile start, Tile goal)
{
	return octileDistance(goal.x - start.x, goal.y - start.y);
}

double PathfindingStateSpace::octileDistance(double deltaX, double deltaY)
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

double PathfindingStateSpace::evaluatedDistance(int stateID, int goalStateID)
{
	Tile state = this->getState(stateID);
	Tile goalState = this->getState(goalStateID);
	return octileDistance(state, goalState);
}

double PathfindingStateSpace::evaluate(int stateID, int goalStateID)
{
	if(goalIdForSavedHeuristic[stateID] != goalStateID) {
		savedHeuristicForTiles[stateID] = evaluatedDistance(stateID, goalStateID);
		goalIdForSavedHeuristic[stateID] = goalStateID;
	}
	return savedHeuristicForTiles[stateID];
}

bool PathfindingStateSpace::isPossibleState(int stateID)
{
	Tile state = getState(stateID);
	return traversibleMap->getMap()[state.getX()][state.getY()];
}

void PathfindingStateSpace::writeStateToStream(int stateID, ostream *out)
{
	this->getState(stateID).writeSelfToStream(out);
}

int PathfindingStateSpace::getStateID(int x, int y)
{
	return x * traversibleMap->getHeight() + y;
}

Tile PathfindingStateSpace::getState(int stateID)
{
	return Tile(stateID / traversibleMap->getHeight(), stateID % traversibleMap->getHeight());
}

}
