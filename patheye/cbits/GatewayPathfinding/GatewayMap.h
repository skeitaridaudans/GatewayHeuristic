/*
 * GatewayMap.h
 *
 *  Created on: Jan 21, 2015
 *      Author: kaha
 */

#ifndef SRC_GATEWAYPATHFINDING_GATEWAYMAP_H_
#define SRC_GATEWAYPATHFINDING_GATEWAYMAP_H_

#include "Gateway.h"
#include <Pathfinding/TraversibleMap.h>
#include <vector>
#include <set>

using namespace std;

namespace GatewayPathfinding {

class GatewayMap {
private:
	int mapWidth;
	int mapHeight;

	bool **map;

	int zoneCount;
	int **zoneMap;
	set<int> *zoneGates;

	int gateCount;
	int **gateMap;
	set<int> *gateZones;
	Gateway *gates;

	double **gateDistances;
	//double ***tileGateDistances;

	TraversibleMap *tMap;

	void readMap(istream &fin);
	void readDistances(string distanceDataFile);

	void calculateGateDistances();
	//void calculateTileGateDistances();
	bool coordsTraversible(int x, int y);
	bool traversibleNonZoneNonGate(int x, int y);

	void rasterizeGateMap();

public:
	GatewayMap();
	GatewayMap(TraversibleMap *tMap);
	GatewayMap(string mapDataFile);
	GatewayMap(istream &fin);

	GatewayMap(string mapDataFile, string distanceDataFile);
	virtual ~GatewayMap();

	void setGates(Gateway *gates, int gateCount);
	Gateway* getGates();

	int getGateCount();
	int getZoneCount();

	void calculateZones();
	void calculateDistances();

	TraversibleMap* getTraversibleMap();

	set<int> getGatesForTile(Tile tile);
	double getDistanceBetweenGates(int gate1, int gate2);
	double getDistanceFromTileToGate(Tile tile, int gate);
	bool areInSameZone(Tile tile1, Tile tile2);
	int gateIdOfTile(Tile tile);

	static vector<Tile> rasterizeGate(Gateway *pGate, int mapWidth, int mapHeight);

	void writeMapData(string mapDataFile);
	void writeDistanceData(string distanceDataFile);
	void printAll(ostream *out);
};

}

#endif /* SRC_GATEWAYPATHFINDING_GATEWAYMAP_H_ */
