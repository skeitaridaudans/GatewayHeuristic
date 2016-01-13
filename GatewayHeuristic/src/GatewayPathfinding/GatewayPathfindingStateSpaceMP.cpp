/*
 * GatewayPathfindingStateSpaceMP.cpp
 *
 *  Created on: Dec 15, 2014
 *      Author: kaha
 */

#include "GatewayPathfindingStateSpaceMP.h"
#include <math.h>

namespace GatewayPathfinding {

GatewayPathfindingStateSpaceMP::GatewayPathfindingStateSpaceMP(GatewayMap *gMap)
: PathfindingStateSpaceMP(gMap->getTraversibleMap())
{
	gatewayMap = gMap;

	lastGoalID = -1;
}


GatewayPathfindingStateSpaceMP::~GatewayPathfindingStateSpaceMP()
{
}

double GatewayPathfindingStateSpaceMP::shortestDistanceToGate(Tile tile, Gateway gateway)
{
	Tile gateStart = gateway.getStart();
	Tile gateEnd = gateway.getEnd();

	int deltaX = 0;
	int deltaY = 0;

	if(gateStart.x == gateEnd.x) {  //Vertical gate
		int higherY, lowerY;
		if(gateStart.y > gateEnd.y) {
			higherY = gateStart.y;
			lowerY = gateEnd.y;
		}
		else {
			higherY = gateEnd.y;
			lowerY = gateStart.y;
		}
		if(tile.y > higherY) {
			deltaY = tile.y - higherY;
		}
		else if(tile.y < lowerY) {
			deltaY = lowerY - tile.y;
		}
		deltaX = tile.x - gateStart.x;
	}
	else if(gateStart.y == gateEnd.y) {  //Horizontal gate
		int higherX, lowerX;
		if(gateStart.x > gateEnd.x) {
			higherX = gateStart.x;
			lowerX = gateEnd.x;
		}
		else {
			higherX = gateEnd.x;
			lowerX = gateStart.x;
		}
		if(tile.x > higherX) {
			deltaX = tile.x - higherX;
		}
		else if(tile.x < lowerX) {
			deltaX = lowerX - tile.x;
		}
		deltaY = tile.y - gateStart.y;
	}
	else {  //Diagonal gate (always 45 degrees)
		int higherX, lowerX;
		int higherY, lowerY;
		bool backslashGate = false;
		if(gateStart.y > gateEnd.y) {
			higherY = gateStart.y;
			lowerY = gateEnd.y;
			if(gateStart.x > gateEnd.x) {
				higherX = gateStart.x;
				lowerX = gateEnd.x;
				backslashGate = false;
			}
			else {
				higherX = gateEnd.x;
				lowerX = gateStart.x;
				backslashGate = true;
			}
		}
		else {
			higherY = gateEnd.y;
			lowerY = gateStart.y;
			if(gateStart.x > gateEnd.x) {
				higherX = gateStart.x;
				lowerX = gateEnd.x;
				backslashGate = true;
			}
			else {
				higherX = gateEnd.x;
				lowerX = gateStart.x;
				backslashGate = false;
			}
		}
		if(backslashGate) {
			if(tile.y > (higherY - (lowerX - tile.x))) {
				deltaX = tile.x - lowerX;
				deltaY = tile.y - higherY;
			}
			else if(tile.y < (lowerY - (higherX - tile.x))) {
				deltaX = higherX - tile.x;
				deltaY = lowerY - tile.y;
			}
			else {
				deltaX = deltaY =
					( (tile.x - lowerX)
					+ (tile.x - higherX)
					+ (tile.y - lowerY)
					+ (tile.y - higherY) )
					/ 4;
			}
		}
		else {
			if(tile.y > (higherY - (tile.x - higherX))) {
				deltaX = tile.x - higherX;
				deltaY = tile.y - higherY;
			}
			else if(tile.y < (lowerY - (tile.x - lowerX))) {
				deltaX = lowerX - tile.x;
				deltaY = lowerY - tile.y;
			}
			else {
				deltaX = deltaY =
					( (tile.x - lowerX)
					+ (tile.x - higherX)
					+ (lowerY - tile.y)
					+ (higherY - tile.y) )
					/ 4;
			}
		}
	}
	return octileDistance(deltaX, deltaY);
}

double GatewayPathfindingStateSpaceMP::evaluatedDistance(int stateID, int goalStateID)
{
	Tile state = this->getState(stateID);
	Tile goalState = this->getState(goalStateID);
	double pureOctileDistance = octileDistance(state, goalState);
	if(gatewayMap->areInSameZone(state, goalState))
	{
		return pureOctileDistance;
	}
	else
	{
		double shortestDistance = -1;
		Gateway *gates = gatewayMap->getGates();
		set<int> stateGates = gatewayMap->getGatesForTile(state);
		set<int> goalGates = gatewayMap->getGatesForTile(goalState);

		if(lastGoalID != goalStateID) {
			goalGateDistances.clear();
			for(set<int>::iterator gGate = goalGates.begin(); gGate != goalGates.end(); ++gGate)
			{
				goalGateDistances.push_back(shortestDistanceToGate(goalState, gates[*gGate]));
			}
			lastGoalID = goalStateID;
		}

		//int sGateNum = 0;
		for(set<int>::iterator sGate = stateGates.begin(); sGate != stateGates.end(); ++sGate)
		{
			double startToGate = shortestDistanceToGate(state, gates[*sGate]);
			//double startToGate = gatewayMap->getDistanceFromTileToGate(state, sGateNum);

			int gGateNum = 0;
			for(set<int>::iterator gGate = goalGates.begin(); gGate != goalGates.end(); ++gGate)
			{
				double gateDist = gatewayMap->getDistanceBetweenGates(*sGate, *gGate);

				//double gateToGoal = gatewayMap->getDistanceFromTileToGate(goalState, gGateNum);
				//double gateToGoal = shortestDistanceToGate(goalState, gates[*gGate]);
				double gateToGoal = goalGateDistances[gGateNum];

				double evaluation = startToGate + gateDist + gateToGoal;
				//evaluation = max(0.0, evaluation - 1.0); //Test fix
				if(evaluation < shortestDistance || shortestDistance == -1)
				{
					if(evaluation <= pureOctileDistance)
					{
						return pureOctileDistance;
					}
					else
					{
						shortestDistance = evaluation;
					}
				}
				++gGateNum;
			}
			//++sGateNum;
		}
		if(shortestDistance == -1) {
			return pureOctileDistance;
		}
		else {
			return shortestDistance;
		}
	}
}

}
