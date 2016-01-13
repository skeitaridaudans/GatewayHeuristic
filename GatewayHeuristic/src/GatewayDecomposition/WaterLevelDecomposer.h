/*
 * WaterLevelDecomposer.h
 *
 *  Created on: Dec 15, 2014
 *      Author: kaha
 */

#ifndef WATERLEVELDECOMPOSER_H_
#define WATERLEVELDECOMPOSER_H_

#include <vector>
#include <Pathfinding/TraversibleMap.h>
#include <GatewayPathfinding/GatewayMap.h>

using namespace std;
using namespace GatewayPathfinding;

namespace GatewayDecomposition {

class WaterLevelDecomposer {
private:
	bool **map;
	int **zoneClusterMap;
	int **gateClusterMap;
	int mapWidth;
	int mapHeight;
	//int gateTrue;

	bool coordsTraversible(int x, int y);
	bool coordsInsideMapAndSomeZone(int x, int y);
	bool traversibleNonZoneNonGate(int x, int y);
	int octileDistance(int deltaX, int deltaY);

public:
	WaterLevelDecomposer();
	virtual ~WaterLevelDecomposer();

	vector<Gateway> decompose(TraversibleMap *tMap, int distanceDenominator, int wallThreshold, int depthThreshold);
};

}

#endif /* WATERLEVELDECOMPOSER_H_ */
