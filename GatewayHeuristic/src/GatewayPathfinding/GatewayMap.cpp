/*
 * GatewayMap.cpp
 *
 *  Created on: Jan 21, 2015
 *      Author: kaha
 */

#include <sstream>
#include "GatewayMap.h"
#include "GateDistanceStateSpace.h"
#include <AStar/AStar.h>
#include <Bitmap/BitmapWriter.h>

#include <fstream>
#include <iostream>

using namespace std;
using namespace Bitmap;

namespace GatewayPathfinding {

GatewayMap::GatewayMap() {
	map = 0;
	tMap = 0;

	mapWidth = 0;
	mapHeight = 0;
	zoneCount = 0;
	gateCount = 0;

	zoneMap = 0;
	gateMap = 0;
	gates = 0;
	gateZones = 0;
	zoneGates = 0;
	gateDistances = 0;
	//tileGateDistances = 0;
}

GatewayMap::GatewayMap(TraversibleMap *tMap) {
	map = tMap->getMap();
	this->tMap = tMap;

	mapWidth = tMap->getWidth();
	mapHeight = tMap->getHeight();
	zoneCount = 0;
	gateCount = 0;

	zoneMap = 0;
	gateMap = 0;
	gates = 0;
	gateZones = 0;
	zoneGates = 0;
	gateDistances = 0;
	//tileGateDistances = 0;
}

GatewayMap::GatewayMap(string mapDataFile) {
	tMap = 0;

	mapWidth = 0;
	mapHeight = 0;
	zoneCount = 0;
	gateCount = 0;

	zoneMap = 0;
	gateMap = 0;
	gates = 0;
	gateZones = 0;
	zoneGates = 0;
	gateDistances = 0;

	readMap(mapDataFile);
}

GatewayMap::GatewayMap(string mapDataFile, string distanceDataFile) {
	tMap = 0;

	mapWidth = 0;
	mapHeight = 0;
	zoneCount = 0;
	gateCount = 0;

	zoneMap = 0;
	gateMap = 0;
	gates = 0;
	gateZones = 0;
	zoneGates = 0;
	gateDistances = 0;

	readMap(mapDataFile);
	readDistances(distanceDataFile);
}

GatewayMap::~GatewayMap() {

	if(tMap != 0)
	{
		delete tMap;
		map = 0;
	}

	for(int i = 0; i < mapWidth; ++i) {
		if(map != 0) {
			delete[] map[i]; }
		delete[] zoneMap[i];
		delete[] gateMap[i];
	}
	if(map != 0) {
		delete[] map; }
	delete[] zoneMap;
	delete[] gateMap;

	delete[] zoneGates;

	delete[] gateZones;
	//delete[] gates;

	if(gateDistances != 0)
	{
		for(int i = 0; i < gateCount; ++i)
		{
			delete[] gateDistances[i];
		}
		delete[] gateDistances;
	}
/*
	for(int i = 0; i < mapWidth; ++i)
	{
		for(int j = 0; j < mapHeight; ++j)
		{
			if(tileGateDistances[i][j] != 0) {
				delete[] tileGateDistances[i][j];
			}
		}
		delete[] tileGateDistances[i];
	}
	delete[] tileGateDistances;*/
}

void GatewayMap::calculateZones()
{
	cout << "Calculating zones and gates...";
	cout.flush();

	rasterizeGateMap();

	int **bmpDrawingMap = new int*[mapWidth];

	for(int i = 0; i < mapWidth; i++)
	{
		bmpDrawingMap[i] = new int[mapHeight];
	}

	for(int i = 0; i < mapWidth; i++)
	{
		for(int j = 0; j < mapHeight; j++)
		{
			if(map[i][j])
			{
				if(gateMap[i][j] != -1)
					bmpDrawingMap[i][j] = 1;
				else
					bmpDrawingMap[i][j] = 2;
			}
			else
			{
				bmpDrawingMap[i][j] = 0;
			}
		}
	}

	stringstream bmpFileName;
	bmpFileName.str("");
	bmpFileName << "bmp/" << tMap->getFileName() << "_D_water_level_processed_gates" << ".bmp";
	BitmapWriter::writeBMP(bmpDrawingMap, mapWidth, mapHeight, 2, bmpFileName.str());


	/************FIX GATES HORIZONTAL/VERTICAL/DIAGONAL************
	 *
	 */

	for(int i = 0; i < gateCount; ++i)
	{
		Gateway *pGate = &gates[i];

		float deltaX = abs(pGate->getEnd().x - pGate->getStart().x);
		float deltaY = abs(pGate->getEnd().y - pGate->getStart().y);

		if(deltaX != 0 && deltaY != 0)
		{
			float slope = deltaY / deltaX;

			if(slope != 1.0) {
				Tile straightStart, straightEnd;
				Tile diagonalStart, diagonalEnd;
				if(slope > 1.0) {
					Tile t1, t2;
					int switchDir = 1;
					if(pGate->getEnd().y < pGate->getStart().y) {
						t1 = pGate->getStart();
						t2 = pGate->getEnd();
					}
					else {
						t1 = pGate->getEnd();
						t2 = pGate->getStart();
					}
					if(t1.x > t2.x) {
						switchDir = -1;
					}
					Tile tmp1;
					Tile tmp2;
					while(t1.x != t2.x) {
						tmp1.x = t1.x;
						tmp1.y = t1.y;
						tmp2.x = t2.x;
						tmp2.y = t2.y;

						tmp1.x += switchDir;
						while(!(coordsTraversible(tmp1.x, tmp1.y) && !coordsTraversible(tmp1.x, tmp1.y + 1))) {
							if(coordsTraversible(tmp1.x, tmp1.y)) {
								tmp1.y++;
							}
							else {
								tmp1.y--;
							}
						}

						tmp2.x -= switchDir;
						while(!(coordsTraversible(tmp2.x, tmp2.y) && !coordsTraversible(tmp2.x, tmp2.y - 1))) {
							if(coordsTraversible(tmp2.x, tmp2.y)) {
								tmp2.y--;
							}
							else {
								tmp2.y++;
							}
						}

						if(t2.y - tmp2.y > tmp1.y - t1.y) {
							t1.x = tmp1.x;
							t1.y = tmp1.y;
						}
						else {
							t2.x = tmp2.x;
							t2.y = tmp2.y;
						}
					}
					straightStart = t1;
					straightEnd = t2;
				}
				else {
					Tile t1, t2;
					int switchDir = 1;
					if(pGate->getEnd().x > pGate->getStart().x) {
						t1 = pGate->getStart();
						t2 = pGate->getEnd();
					}
					else {
						t1 = pGate->getEnd();
						t2 = pGate->getStart();
					}
					if(t1.y < t2.y) {
						switchDir = -1;
					}
					Tile tmp1;
					Tile tmp2;
					while(t1.y != t2.y) {
						tmp1.y = t1.y;
						tmp1.x = t1.x;
						tmp2.y = t2.y;
						tmp2.x = t2.x;

						tmp1.y -= switchDir;
						while(!(coordsTraversible(tmp1.x, tmp1.y) && !coordsTraversible(tmp1.x - 1, tmp1.y))) {
							if(coordsTraversible(tmp1.x, tmp1.y)) {
								tmp1.x--;
								if(tmp1.x < 0) {
									tmp1.x = 0;
									break;
								}
							}
							else {
								tmp1.x++;
								if(tmp1.x >= mapWidth) {
									tmp1.x = mapWidth - 1;
									break;
								}
							}
						}

						tmp2.y += switchDir;
						while(!(coordsTraversible(tmp2.x, tmp2.y) && !coordsTraversible(tmp2.x + 1, tmp2.y))) {
							if(coordsTraversible(tmp2.x, tmp2.y)) {
								tmp2.x++;
								if(tmp2.x >= mapWidth) {
									tmp2.x = mapWidth - 1;
									break;
								}
							}
							else {
								tmp2.x--;
								if(tmp2.x < 0) {
									tmp2.x = 0;
									break;
								}
							}
						}

						if(tmp2.x - t2.x > t1.x - tmp1.x) {
							t1.y = tmp1.y;
							t1.x = tmp1.x;
						}
						else {
							t2.y = tmp2.y;
							t2.x = tmp2.x;
						}
					}
					straightStart = t1;
					straightEnd = t2;
				}

				Tile t1, t2;
				int switchDirX = 1;
				int switchDirY = 1;
				if(pGate->getEnd().y < pGate->getStart().y) {
					t1 = pGate->getStart();
					t2 = pGate->getEnd();
				}
				else {
					t1 = pGate->getEnd();
					t2 = pGate->getStart();
				}
				if(t1.x > t2.x) {
					if(slope < 1.0) {
						switchDirX = -1;
					}
					else {
						switchDirY = -1;
					}
				}
				else if(slope > 1.0) {
					switchDirX = -1;
					switchDirY = -1;
				}
				int parallelDir = -(switchDirX * switchDirY);
				bool changeX = true;
				Tile tmp1;
				Tile tmp2;
				while(abs(t1.x - t2.x) != abs(t1.y - t2.y)) {
					tmp1.x = t1.x;
					tmp1.y = t1.y;
					tmp2.x = t2.x;
					tmp2.y = t2.y;

					if(changeX)	{ tmp1.x += switchDirX; }
					else		{ tmp1.y += switchDirY; }
					while(!(coordsTraversible(tmp1.x, tmp1.y) && !coordsTraversible(tmp1.x + parallelDir, tmp1.y + 1))) {
						if(coordsTraversible(tmp1.x, tmp1.y)) {
							tmp1.y++;
							tmp1.x += parallelDir;
						}
						else {
							tmp1.y--;
							tmp1.x -= parallelDir;
						}
					}

					if(changeX)	{ tmp2.x -= switchDirX; }
					else		{ tmp2.y -= switchDirY; }
					while(!(coordsTraversible(tmp2.x, tmp2.y) && !coordsTraversible(tmp2.x - parallelDir, tmp2.y - 1))) {
						if(coordsTraversible(tmp2.x, tmp2.y)) {
							tmp2.y--;
							tmp2.x -= parallelDir;
						}
						else {
							tmp2.y++;
							tmp2.x += parallelDir;
						}
					}

					if(t2.y - tmp2.y > tmp1.y - t1.y) {
						t1.x = tmp1.x;
						t1.y = tmp1.y;
					}
					else {
						t2.x = tmp2.x;
						t2.y = tmp2.y;
					}
					changeX = !changeX;
				}
				diagonalStart = t1;
				diagonalEnd = t2;

				int straightLength;
				if(straightStart.x == straightEnd.x) {
					straightLength = abs(straightEnd.y - straightStart.y);
				}
				else {
					straightLength = abs(straightEnd.x - straightStart.x);
				}
				if((double)straightLength <= (double)abs(diagonalEnd.x - diagonalStart.x) * 1.4) {
					pGate->setStart(straightStart);
					pGate->setEnd(straightEnd);
				}
				else {
					pGate->setStart(diagonalStart);
					pGate->setEnd(diagonalEnd);
				}
			}
		}
	}


	rasterizeGateMap();

	cout << "GateClusterMap fixed...";
	cout.flush();

	for(int i = 0; i < mapWidth; i++)
	{
		for(int j = 0; j < mapHeight; j++)
		{
			if(map[i][j])
			{
				if(gateMap[i][j] != -1)
					bmpDrawingMap[i][j] = 1;
				else
					bmpDrawingMap[i][j] = 2;
			}
			else
			{
				bmpDrawingMap[i][j] = 0;
			}
		}
	}

	bmpFileName.str("");
	bmpFileName << "bmp/" << tMap->getFileName() << "_E_water_level_fixed_gates" << ".bmp";
	BitmapWriter::writeBMP(bmpDrawingMap, mapWidth, mapHeight, 2, bmpFileName.str());


	/************REFINE ZONE CLUSTER MAP AND BUILD CONNECTIONS************
	 *
	 */

	vector<set<int> > v_zoneGates;
	vector<set<int> > v_gateZones;
	for(int i = 0; i < gateCount; i++)
	{
		v_gateZones.push_back(set<int>());
	}

	if(zoneMap == 0) {
		zoneMap = new int*[mapWidth];
		for(int i = 0; i < mapWidth; ++i)
		{
			zoneMap[i] = new int[mapHeight];
		}
	}

	for(int i = 0; i < mapWidth; ++i)
	{
		for(int j = 0; j < mapHeight; ++j)
		{
			zoneMap[i][j] = -1;
		}
	}

	int currentClusterID = 0;

	for(int i = 0; i < mapWidth; ++i)
	{
		for(int j = 0; j < mapHeight; ++j)
		{
			if(traversibleNonZoneNonGate(i,j))
			{
				set<int> currentZoneGates;
				//Flood fill zone with new clusterID
				queue<Tile> Q;
				Q.push(Tile(i, j));
				while(!Q.empty())
				{
					Tile node = Q.front();
					Q.pop();
					int x = node.getX();
					int y = node.getY();
					if(zoneMap[x][y] == -1)
					{
						int w, e; //w west, e east
						for(w = x; w > 0 && traversibleNonZoneNonGate(w-1, y); --w){}
						for(e = x; e+1 < mapWidth && traversibleNonZoneNonGate(e+1, y); ++e){}
					//Someone has to get the gate tiles themselves
						if(e+1 < mapWidth && gateMap[e+1][y] != -1) {
							zoneMap[e+1][y] = currentClusterID;
							currentZoneGates.insert(gateMap[e+1][y]);
							v_gateZones[gateMap[e+1][y]].insert(currentClusterID);
						}
						if(w > 0 && gateMap[w-1][y] != -1) {
							zoneMap[w-1][y] = currentClusterID;
							currentZoneGates.insert(gateMap[w-1][y]);
							v_gateZones[gateMap[w-1][y]].insert(currentClusterID);
						}
					//...
						for(int h = w; h <= e; ++h)
						{
							zoneMap[h][y] = currentClusterID;

							if(y > 0) {
								if(traversibleNonZoneNonGate(h, y-1)) {
									Q.push(Tile(h, y-1));
								}
								//Someone has to get the gate tiles themselves
								else if(gateMap[h][y-1] != -1){
									zoneMap[h][y-1] = currentClusterID;
									currentZoneGates.insert(gateMap[h][y-1]);
									v_gateZones[gateMap[h][y-1]].insert(currentClusterID);
								}
							}
							if(y+1 < mapHeight) {
								if(traversibleNonZoneNonGate(h, y+1)) {
									Q.push(Tile(h, y+1));
								}
								//Someone has to get the gate tiles themselves
								else if(gateMap[h][y+1] != -1) {
									zoneMap[h][y+1] = currentClusterID;
									currentZoneGates.insert(gateMap[h][y+1]);
									v_gateZones[gateMap[h][y+1]].insert(currentClusterID);
								}
							}
						}
					}
				}
				v_zoneGates.push_back(currentZoneGates);
				++currentClusterID;
			}
		}
	}

	zoneCount = currentClusterID;

	cout << "ZoneMap refined...";
	cout.flush();

	for(int i = 0; i < mapWidth; i++)
	{
		for(int j = 0; j < mapHeight; j++)
		{
			if(map[i][j])
			{
				bmpDrawingMap[i][j] = zoneMap[i][j] + 20;
			}
			else
			{
				bmpDrawingMap[i][j] = 0;
			}
		}
	}

	bmpFileName.str("");
	bmpFileName << "bmp/" << tMap->getFileName() << "_F_water_level_refined_zones" << ".bmp";
	BitmapWriter::writeBMP(bmpDrawingMap, mapWidth, mapHeight, currentClusterID + 20, bmpFileName.str());


	/************REMOVE USELESS GATES************
	 *
	 */
/*
	int gateNum = 0;
	vector<set<int> >::iterator currentGateZones = v_gateZones.begin();

	while(currentGateZones != v_gateZones.end())
	{
		if(currentGateZones->size() < 2)
		{
			for(set<int>::iterator it = currentGateZones->begin(); it != currentGateZones->end(); ++it)
			{
				v_zoneGates.at(*it).erase(gateNum);
			}
			currentGateZones = v_gateZones.erase(currentGateZones);
			--gateCount;
			for(int i = gateNum; i < gateCount; ++i) {
				gates[i] = gates[i+1];
			}

			for(vector<set<int> >::iterator currentZoneGates = v_zoneGates.begin(); currentZoneGates != v_zoneGates.end(); ++currentZoneGates)
			{
				for(set<int>::iterator it = currentZoneGates->begin(); it != currentZoneGates->end();)
				{
					if(*it > gateNum)
					{
						int gateToDecrease = *it;
						++it;
						currentZoneGates->erase(gateToDecrease);
						currentZoneGates->insert(gateToDecrease - 1);
					}
					else
					{
						++it;
					}
				}
			}
		}
		else
		{
			++currentGateZones;
		}
	}

	rasterizeGateMap();  //build gateMap again to get correct indices in the map structure
*/


	/************BUILD THE GATEWAY MAP STRUCTURE************
	 *
	 */

	if(gateZones != 0) {
		delete[] gateZones;
	}
	if(zoneGates != 0) {
		delete[] zoneGates;
	}

	gateZones = new set<int>[gateCount];
	int zoneCount = v_zoneGates.size();
	zoneGates = new set<int>[zoneCount];

	for(int i = 0; i < gateCount; i++)
	{
		for(set<int>::iterator it = v_gateZones[i].begin(); it != v_gateZones[i].end(); it++)
		{
			gateZones[i].insert(*it);
		}
	}
	for(int i = 0; i < zoneCount; i++)
	{
		for(set<int>::iterator it = v_zoneGates[i].begin(); it != v_zoneGates[i].end(); it++)
		{
			zoneGates[i].insert(*it);
		}
	}

	cout << "Zones and gates calculated!" << endl;
}

void GatewayMap::calculateDistances()
{
	calculateGateDistances();
	//calculateTileGateDistances();
}

void GatewayMap::calculateGateDistances()
{
	GateDistanceStateSpace stateSpace(this);
	gateDistances = new double*[gateCount];
	for(int i = 0; i < gateCount; i++)
	{
		gateDistances[i] = new double[gateCount];
		for(int j = 0; j < gateCount; j++)
		{
			gateDistances[i][j] = -1;
		}
		stateSpace.setResultArray(gateDistances[i]);

		cout << "Calculating distances for gate " << i << " / " << gateCount << endl;

		vector<Tile> tiles = rasterizeGate(&(gates[i]), mapWidth, mapHeight);
		vector<int> stateIDs;
		for(vector<Tile>::iterator pTile = tiles.begin(); pTile != tiles.end(); ++pTile)
		{
			stateIDs.push_back(stateSpace.getStateID(pTile->getX(), pTile->getY()));
		}
		AStar::AStar astar(&stateSpace, stateIDs);

		astar.findMultipleSolutions();
	}
}
/*
void GatewayMap::calculateTileGateDistances()
{
	GateDistanceStateSpace stateSpace(this);
	tileGateDistances = new double**[mapWidth];
	for(int i = 0; i < mapWidth; ++i)
	{
		cout << "Calculating distances for column " << i << " / " << mapWidth << endl;
		tileGateDistances[i] = new double*[mapHeight];
		for(int j = 0; j < mapHeight; ++j)
		{
			if(zoneMap[i][j] == -1) {
				tileGateDistances[i][j] = 0;
			}
			else {
				tileGateDistances[i][j] = new double[zoneGates[zoneMap[i][j]].size()];
				for(unsigned int n = 0; n < zoneGates[zoneMap[i][j]].size(); ++n) {
					tileGateDistances[i][j][n] = -1;
				}
				stateSpace.setResultArray(tileGateDistances[i][j]);
				stateSpace.setSearchGateIDs(&zoneGates[zoneMap[i][j]]);

				AStar::AStar astar(&stateSpace, stateSpace.getStateID(i, j), 0);
				astar.findMultipleSolutions();
			}
		}
	}
}
*/
TraversibleMap* GatewayMap::getTraversibleMap()
{
	if(map == 0)
	{
		map = new bool*[mapWidth];
		for(int i = 0; i < mapWidth; i++)
		{
			map[i] = new bool[mapHeight];
			for(int j = 0; j < mapHeight; j++)
			{
				if(zoneMap[i][j] == -1) {
					map[i][j] = false; }
				else {
					map[i][j] = true; }
			}
		}
	}
	if(tMap == 0)
	{
		tMap = new TraversibleMap(map, mapWidth, mapHeight);
	}
	return tMap;
}

set<int> GatewayMap::getGatesForTile(Tile tile)
{
	int x = tile.getX();
	int y = tile.getY();

	int index = zoneMap[x][y];
	if(index < 0)
	{
		return set<int>();
	}

	return zoneGates[index];
	//return zoneGates[zoneMap[tile.getX()][tile.getY()]];
}

double GatewayMap::getDistanceBetweenGates(int gate1, int gate2)
{
	return gateDistances[gate1][gate2];
}
/*
double GatewayMap::getDistanceFromTileToGate(Tile tile, int gate)
{
	return tileGateDistances[tile.getX()][tile.getY()][gate];
}
*/
bool GatewayMap::areInSameZone(Tile tile1, Tile tile2)
{
	return zoneMap[tile1.getX()][tile1.getY()] == zoneMap[tile2.getX()][tile2.getY()];
}

int GatewayMap::gateIdOfTile(Tile tile)
{
	return gateMap[tile.getX()][tile.getY()];
}

vector<Tile> GatewayMap::rasterizeGate(Gateway *pGate, int mapWidth, int mapHeight)
{
	vector<Tile> rasterizedTiles;

	int x1 = pGate->getStart().getX();
	int y1 = pGate->getStart().getY();
	int x2 = pGate->getEnd().getX();
	int y2 = pGate->getEnd().getY();

	if(abs(x2-x1) > abs(y2-y1))
	{
		if(x1 > x2) //swap
		{
			int tmp = x1;
			x1 = x2;
			x2 = tmp;
			tmp = y1;
			y1 = y2;
			y2 = tmp;
		}
		//rasterize vertical scanlines
		for(int x = x1; x <= x2; x++)
		{
			float t = (x2 - x1) > 0 ? (float)(x - x1) / (float)(x2 - x1) : 0;
			int y = (1-t) * y1 + t * y2 + 0.5;
			if(x >= 0 && y >= 0 && x < mapWidth && y < mapHeight)
			{
				rasterizedTiles.push_back(Tile(x, y));
			}
		}
	}
	else
	{
		if(y1 > y2) //swap
		{
			int tmp = x1;
			x1 = x2;
			x2 = tmp;
			tmp = y1;
			y1 = y2;
			y2 = tmp;
		}
		//rasterize horizontal scanlines
		for(int y = y1; y <= y2; y++)
		{
			float t = (y2 - y1) > 0 ? (float)(y - y1) / (float)(y2 - y1) : 0;
			int x = (1-t) * x1 + t * x2 + 0.5;
			if(x >= 0 && y >= 0 && x < mapWidth && y < mapHeight)
			{
				rasterizedTiles.push_back(Tile(x, y));
			}
		}
	}
	return rasterizedTiles;
}

void GatewayMap::printAll(ostream *out)
{
	*out << "Gates" << endl;
	for(int i = 0; i < gateCount; i++)
	{
		*out << i << ": ";
		gates[i].writeSelfToStream(out);
		*out << "     \t";
		for(set<int>::iterator it = gateZones[i].begin(); it != gateZones[i].end(); ++it)
		{
			*out << *it << " ";
		}
		*out << endl;
	}

	*out << "ZoneGates" << endl;
	for(int i = 0; i < zoneCount; i++)
	{
		*out << i << ": ";
		for(set<int>::iterator it = zoneGates[i].begin(); it != zoneGates[i].end(); ++it)
		{
			*out << *it << " ";
		}
		*out << endl;
	}

	if(gateDistances != 0)
	{
		*out << "GateDistances" << endl;
		{
			for(int i = 0; i < gateCount; ++i)
			{
				*out << "\t" << i;
			}
			*out << endl;
			for(int j = 0; j < gateCount; ++j)
			{
				*out << j;
				for(int i = 0; i < gateCount; ++i)
				{
					*out << "\t" << gateDistances[i][j];
				}
				*out << endl;
			}
		}
	}
}

void GatewayMap::setGates(Gateway *gates, int gateCount) {
	this->gates = gates;
	this->gateCount = gateCount;
}

void GatewayMap::rasterizeGateMap() {
	if(gateMap == 0) {
		gateMap = new int*[mapWidth];
		for(int i = 0; i < mapWidth; ++i)
		{
			gateMap[i] = new int[mapHeight];
		}
	}
	for(int i = 0; i < mapWidth; ++i)
	{
		for(int j = 0; j < mapHeight; ++j)
		{
			gateMap[i][j] = -1;
		}
	}

	for(int currentGate = 0; currentGate < gateCount; currentGate++)
	{
		vector<Tile> tiles = rasterizeGate(&gates[currentGate], mapWidth, mapHeight);
		for(vector<Tile>::iterator pTile = tiles.begin(); pTile != tiles.end(); ++pTile)
		{
			gateMap[pTile->getX()][pTile->getY()] = currentGate;
		}
	}
}

void GatewayMap::writeMapData(string mapDataFile)
{
	ofstream fout;
	fout.open(mapDataFile.c_str(), ios::binary);
	fout.seekp(fout.beg);

	fout.write((char*)&mapWidth, sizeof(int));
	fout.write((char*)&mapHeight, sizeof(int));
	for(int i = 0; i < mapWidth; ++i)
	{
		fout.write((char*)map[i], mapHeight * sizeof(bool));
	}

	fout.write((char*)&zoneCount, sizeof(int));
	for(int i = 0; i < mapWidth; ++i)
	{
		fout.write((char*)zoneMap[i], mapHeight * sizeof(int));
	}
	for(int i = 0; i < zoneCount; i++)
	{
		int c = zoneGates[i].size();
		fout.write((char*)&c, sizeof(int));
		for(set<int>::iterator it = zoneGates[i].begin(); it != zoneGates[i].end(); ++it)
		{
			fout.write((char*)&(*it), sizeof(int));
		}
	}

	fout.write((char*)&gateCount, sizeof(int));
	for(int i = 0; i < mapWidth; ++i)
	{
		fout.write((char*)gateMap[i], mapHeight * sizeof(int));
	}
	for(int i = 0; i < gateCount; i++)
	{
		int c = gateZones[i].size();
		fout.write((char*)&c, sizeof(int));
		for(set<int>::iterator it = gateZones[i].begin(); it != gateZones[i].end(); ++it)
		{
			fout.write((char*)&(*it), sizeof(int));
		}
	}

	int *values = new int[4 * gateCount];
	for(int i = 0; i < gateCount; i++)
	{
		values[i*4] = gates[i].start.x;
		values[i*4+1] = gates[i].start.y;
		values[i*4+2] = gates[i].end.x;
		values[i*4+3] = gates[i].end.y;
	}
	fout.write((char*)values, gateCount * 4 * sizeof(int));
}

void GatewayMap::writeDistanceData(string distanceDataFile)
{
	ofstream fout;
	fout.open(distanceDataFile.c_str(), ios::binary);
	fout.seekp(fout.beg);

	fout.write((char*)&gateCount, sizeof(int));
	for(int i = 0; i < gateCount; i++)
	{
		fout.write((char*)gateDistances[i], gateCount * sizeof(double));
	}
/*
	for(int i = 0; i < mapWidth; ++i)
	{
		for(int j = 0; j < mapHeight; ++j)
		{
			if(tileGateDistances[i][j] != 0) {
				fout.write((char*)tileGateDistances[i][j],
						zoneGates[zoneMap[i][j]].size() * sizeof(double));
			}
		}
	}*/
}

void GatewayMap::readMap(string mapDataFile)
{
	ifstream fin;
	fin.open(mapDataFile.c_str(), ios::binary);
	fin.seekg(fin.beg);

	fin.read((char*)&mapWidth, sizeof(int));
	fin.read((char*)&mapHeight, sizeof(int));
	map = new bool*[mapWidth];
	for(int i = 0; i < mapWidth; ++i)
	{
		map[i] = new bool[mapHeight];
		fin.read((char*)map[i], mapHeight * sizeof(bool));
	}

	fin.read((char*)&zoneCount, sizeof(int));
	zoneMap = new int*[mapWidth];
	for(int i = 0; i < mapWidth; ++i)
	{
		zoneMap[i] = new int[mapHeight];
		fin.read((char*)zoneMap[i], mapHeight * sizeof(int));
	}
	zoneGates = new set<int>[zoneCount];
	for(int i = 0; i < zoneCount; ++i)
	{
		int c;
		fin.read((char*)&c, sizeof(int));
		for(int zone = 0; zone < c; ++zone)
		{
			int gate;
			fin.read((char*)&gate, sizeof(int));
			zoneGates[i].insert(gate);
		}
	}

	fin.read((char*)&gateCount, sizeof(int));
	gateMap = new int*[mapWidth];
	for(int i = 0; i < mapWidth; ++i)
	{
		gateMap[i] = new int[mapHeight];
		fin.read((char*)gateMap[i], mapHeight * sizeof(int));
	}
	gateZones = new set<int>[gateCount];
	for(int i = 0; i < gateCount; ++i)
	{
		int c;
		fin.read((char*)&c, sizeof(int));
		for(int gate = 0; gate < c; ++gate)
		{
			int zone;
			fin.read((char*)&zone, sizeof(int));
			gateZones[i].insert(zone);
		}
	}

	gates = new Gateway[gateCount];
	int *values = new int[4 * gateCount];
	fin.read((char*)values, gateCount * 4 * sizeof(int));
	for(int i = 0; i < gateCount; i++)
	{
		gates[i].start.x = values[i*4];
		gates[i].start.y = values[i*4+1];
		gates[i].end.x = values[i*4+2];
		gates[i].end.y = values[i*4+3];
	}
}

void GatewayMap::readDistances(string distanceDataFile)
{
	ifstream fin;
	fin.open(distanceDataFile.c_str(), ios::binary);
	fin.seekg(fin.beg);

	int fileGateCount;
	fin.read((char*)&fileGateCount, sizeof(int));
	if(fileGateCount != gateCount)
	{
		cout << "Gate count in gate distance file doesn't match gate count in GatewayMap" << endl;
		return;
	}
	gateDistances = new double*[gateCount];
	for(int i = 0; i < gateCount; i++)
	{
		gateDistances[i] = new double[gateCount];
		fin.read((char*)gateDistances[i], gateCount * sizeof(double));
	}
/*
	tileGateDistances = new double**[mapWidth];
	for(int i = 0; i < mapWidth; ++i)
	{
		tileGateDistances[i] = new double*[mapHeight];
		for(int j = 0; j < mapHeight; ++j)
		{
			if(zoneMap[i][j] == -1) {
				tileGateDistances[i][j] = 0;
			}
			else {
				tileGateDistances[i][j] = new double[zoneGates[zoneMap[i][j]].size()];
				fin.read((char*)tileGateDistances[i][j],
						zoneGates[zoneMap[i][j]].size() * sizeof(double));
			}
		}
	}*/
}

Gateway* GatewayMap::getGates() {
	return gates;
}

int GatewayMap::getGateCount() {
	return gateCount;
}

int GatewayMap::getZoneCount() {
	return zoneCount;
}

bool GatewayMap::coordsTraversible(int x, int y)
{
	return x >= 0 && x < mapWidth && y >= 0 && y < mapHeight && map[x][y];
}

bool GatewayMap::traversibleNonZoneNonGate(int x, int y)
{
	return map[x][y] && zoneMap[x][y] == -1 && gateMap[x][y] == -1;
}

}
