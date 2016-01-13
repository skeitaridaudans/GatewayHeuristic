/*
 * WaterLevelDecomposer.cpp
 *
 *  Created on: Dec 15, 2014
 *      Author: kaha
 */

#include <iostream>
#include <sstream>
#include "WaterLevelDecomposer.h"
#include <Bitmap/BitmapWriter.h>
#include <list>
#include <queue>
#include <math.h>

using namespace std;
using namespace GatewayPathfinding;
using namespace Bitmap;

namespace GatewayDecomposition {

WaterLevelDecomposer::WaterLevelDecomposer() {
	// TODO Auto-generated constructor stub

	mapHeight = 0;
	mapWidth = 0;

	map = 0;
	zoneClusterMap = 0;
	gateClusterMap = 0;
}

WaterLevelDecomposer::~WaterLevelDecomposer() {
	// TODO Auto-generated destructor stub
}

#define	SINGLE_TILE_DISTANCE	5
#define	DIAGONAL_ADDITION		2

vector<Gateway> WaterLevelDecomposer::decompose(TraversibleMap *tMap, int distanceDenominator, int wallThreshold, int depthThreshold)
{
	/************SET THRESHOLDS************
	 *
	 */

	bool dynamicDistanceDenominator = false;
	bool dynamicWallThreshold = false;
	bool dynamicDepthThreshold = false;
	if(distanceDenominator <= 0)
	{
		distanceDenominator = 1;
		dynamicDistanceDenominator = true;
	}
	if(wallThreshold <= 0)
	{
		wallThreshold = 1;
		dynamicWallThreshold = true;
	}
	if(depthThreshold <= 0)
	{
		depthThreshold = 1;
		dynamicDepthThreshold = true;
	}


	/************INITIALIZE BASE MAP************
	 *
	 */

	map = tMap->getMap();
	mapWidth = tMap->getWidth();
	mapHeight = tMap->getHeight();

	//clock_t startTime = clock();
	cout << "Begin decomposition on " << tMap->getFileName() << "...";
	cout.flush();


	/************INITIALIZE BMP DRAWING MAP************
	 *
	 */

	int **bmpDrawingMap = new int*[mapWidth];

	for(int i = 0; i < mapWidth; i++)
	{
		bmpDrawingMap[i] = new int[mapHeight];

		for(int j = 0; j < mapHeight; j++)
		{
			bmpDrawingMap[i][j] = map[i][j] ? 1 : 0;
		}
	}

	stringstream bmpFileName;
	bmpFileName << "bmp/" << tMap->getFileName() << ".original" << ".bmp";
	BitmapWriter::writeBMP(bmpDrawingMap, mapWidth, mapHeight, 1, bmpFileName.str());

	/************BUILD CIRCLE MAP************
	 *
	 */

	vector<vector<Tile> > circleMap;
	int maxDist = SINGLE_TILE_DISTANCE * ((min(mapWidth, mapHeight) / 2) + 1);

	for(int i = 0; i <= maxDist; i++)
	{
		circleMap.insert(circleMap.end(), vector<Tile>());
	}

	for(int i = 0; i <= maxDist; i++)
	{
		for(int j = 1; j <= maxDist; j++)
		{
			int distance = octileDistance(i, j);
			if(distance <= maxDist)
			{
				circleMap[distance].insert(circleMap[distance].end(), Tile(i, j));
			}
		}
	}

	cout << "CircleMap built...";
	cout.flush();

/*
	for(int i = 0; i <= maxDist; i++)
	{
		cout << "Tiles at distance " << i << ":";
		for(unsigned int j = 0; j < circleMap[i].size(); j++)
		{
			cout << " ";
			circleMap[i][j].writeSelfToStream(&cout);
		}
		cout << endl;
	}
*/



	/************BUILD DEPTH MAP************
	 *
	 */

	int **depthMap = new int*[mapWidth];

	int maxDepthUsed = 0;

	for(int i = 0; i < mapWidth; i++)
	{
		depthMap[i] = new int[mapHeight];

		int distance = 0;

		for(int j = 0; j < mapHeight; j++)
		{
			int counter = 0;
			int hitFirstWallDist = 0;

			if(map[i][j])
			{
				bool hitWall = false;
				depthMap[i][j] = maxDist;
				while(!hitWall)
				{
					// Use circleMap to fill in height for tile
					for(vector<Tile>::iterator it = circleMap[distance].begin(); it != circleMap[distance].end(); ++it)
					{
						int cX = it->getX(); //circleX
						int cY = it->getY(); //circleY
						//			Check all 4 quadrants!!!
						for(int quadrant = 0; quadrant < 4; ++quadrant)
						{
							int tmpX = cX; cX = -cY; cY = tmpX;
							int mX = i + cX; //mapX
							int mY = j + cY; //mapY
							//			Either wall or end of map!
							if(mX < 0 || mX >= mapWidth || mY < 0 || mY >= mapHeight || !map[mX][mY])
							{
								if(counter == 0)
								{
									hitFirstWallDist = distance;
									if(dynamicWallThreshold)
									{
										wallThreshold = (maxDist - distance) / 28 + 1;
									}
								}
								++counter;
								if(counter == wallThreshold)
								{
									if(dynamicDistanceDenominator)
									{
										depthMap[i][j] = log2(distance) + 1;
									}
									else
									{
										depthMap[i][j] = distance / distanceDenominator + 1;
									}
									if(maxDepthUsed < depthMap[i][j]){ maxDepthUsed = depthMap[i][j]; };
									hitWall = true;
								}
							}
						}
						if(hitWall){ break; }
					}
					++distance;
				}
				//			End by reducing current circle by 1
				distance = hitFirstWallDist - (SINGLE_TILE_DISTANCE);
				//distance = 0;
			}
			else
			{
				depthMap[i][j] = 0;
			}
		}
	}

	cout << "DepthMap ready...";
	cout.flush();

	if(dynamicDistanceDenominator)
	{
		distanceDenominator = 0;
	}
	if(dynamicWallThreshold)
	{
		wallThreshold = 0;
	}
	if(dynamicDepthThreshold)
	{
		depthThreshold = 0;
	}


	for(int i = 0; i < mapWidth; i++)
	{
		for(int j = 0; j < mapHeight; j++)
		{
			bmpDrawingMap[i][j] = depthMap[i][j];
		}
	}


	bmpFileName.str("");
	bmpFileName << "bmp/" << tMap->getFileName() << "." << distanceDenominator << "_" << wallThreshold << "_" << depthThreshold << "_A_water_level_depth" << ".bmp";
	BitmapWriter::writeBMP(bmpDrawingMap, mapWidth, mapHeight, maxDepthUsed, bmpFileName.str());

	/************PSEUDO CODE************
	//Build the gateways:
	//Make zoneClusterMap (int **)
	//Make clusterBondMap (unordered_map<pair<int, int>, vector<Tile> >)
	//Start waterLevel at maxDepthUsed and iterate to zero
		//Find all Tiles at that depth and put them in currentDepthTiles (collection with constant time removal)
		//While currentDepthTiles is not empty
			//For each tile in currentDepthTiles
				//Collect clusterIDs of neighboring tiles (using zoneClusterMap)
				//if one neighboring clusterID
					//give same clusterID to tile (in zoneClusterMap)
					//remove tile from currentDepthTiles
				//if two neighboring clusterIDs
					//put Tile in clusterBondMap with the two clusterIDs as key ?????
					//remove tile from currentDepthTiles
			//if no tile was affected in the For-statement this pass
				//pick a tile from currentDepthTiles
				//give it a new clusterID
				//remove it from currentDepthTiles
*/


	/************BUILD TILE DEPTH MAP************
	 *
	 */

	list<Tile> *tileDepthMap = new list<Tile>[maxDepthUsed + 1];
	for(int i = 0; i < mapWidth; ++i)
	{
		for(int j = 0; j < mapHeight; ++j)
		{
			if(depthMap[i][j] > 0)
			{
				tileDepthMap[depthMap[i][j]].push_back(Tile(i, j));
			}
		}
	}


	/************BUILD ZONE CLUSTER MAP************
	 *
	 */

	zoneClusterMap = depthMap; //Reusing memory allocation instead of new int*[mapWidth];
	for(int i = 0; i < mapWidth; ++i)
	{
		//zoneClusterMap[i] = new int[mapHeight];
		for(int j = 0; j < mapHeight; ++j)
		{
			zoneClusterMap[i][j] = -1;
		}
	}

	int currentClusterID = 0;

	for(int waterLevel = maxDepthUsed; waterLevel >= 0; --waterLevel)
	{
		list<Tile> *currentDepthTiles = &(tileDepthMap[waterLevel]);
		while(!currentDepthTiles->empty())
		{
			vector<pair<int, list<Tile>::iterator> > tilesWithNeighbors;
			for(list<Tile>::iterator pTile = currentDepthTiles->begin(); pTile != currentDepthTiles->end(); ++pTile)
			{
				int neighborClusterID = -1;
				int dX = 1; //directionX
				int dY = 0; //directionY
				for(int direction = 0; direction < 4; ++direction)
				{
					int tmpX = dX; dX = -dY; dY = tmpX;
					int nX = pTile->getX() + dX; //neighborX
					int nY = pTile->getY() + dY; //neighborY
					if(coordsInsideMapAndSomeZone(nX, nY))
					{
						if(neighborClusterID == -1)
						{
							neighborClusterID = zoneClusterMap[nX][nY];
						}
						else if(neighborClusterID != zoneClusterMap[nX][nY])
						{
							break;
						}
					}
				}
				if(neighborClusterID != -1)
				{
					tilesWithNeighbors.push_back(pair<int, list<Tile>::iterator>(neighborClusterID, pTile));
				}
			}
			if(!tilesWithNeighbors.empty())
			{
				while(!tilesWithNeighbors.empty())
				{
					list<Tile>::iterator pTile = tilesWithNeighbors.back().second;
					zoneClusterMap[pTile->getX()][pTile->getY()] = tilesWithNeighbors.back().first;
					currentDepthTiles->erase(pTile);
					tilesWithNeighbors.pop_back();
				}
			}
			else
			{
				//pick any tile and assign new clusterID
				Tile tile = currentDepthTiles->front();
				currentDepthTiles->pop_front();
				zoneClusterMap[tile.getX()][tile.getY()] = currentClusterID;
				++currentClusterID;
			}
		}
	}

	cout << "ZoneMap ready...";
	cout.flush();

	for(int i = 0; i < mapWidth; i++)
	{
		for(int j = 0; j < mapHeight; j++)
		{
			if(map[i][j])
			{
				bmpDrawingMap[i][j] = zoneClusterMap[i][j] + 100;
			}
			else
			{
				bmpDrawingMap[i][j] = 0;
			}
		}
	}

	bmpFileName.str("");
	bmpFileName << "bmp/" << tMap->getFileName() << "." << distanceDenominator << "_" << wallThreshold << "_" << depthThreshold << "_B_water_level_zones" << ".bmp";
	//BitmapWriter::writeBMP(bmpDrawingMap, mapWidth, mapHeight, currentClusterID + 100, bmpFileName.str());



	/************BUILD GATE CLUSTER MAP************
	 *
	 */

	gateClusterMap = new int*[mapWidth];
	for(int i = 0; i < mapWidth; ++i)
	{
		gateClusterMap[i] = new int[mapHeight];
		for(int j = 0; j < mapHeight; ++j)
		{
			gateClusterMap[i][j] = -1;
			if(map[i][j])
			{
				int dX = 1; //directionX
				int dY = 0; //directionY
				for(int direction = 0; direction < 4; ++direction)
				{
					int tmpX = dX; dX = -dY; dY = tmpX;
					int nX = i + dX; //neighborX
					int nY = j + dY; //neighborY
					if(coordsInsideMapAndSomeZone(nX, nY))
					{
						if(zoneClusterMap[nX][nY] != zoneClusterMap[i][j])
						{
							gateClusterMap[i][j] = 1;
						}
					}
				}
			}
		}
	}

	cout << "GateClusterMap ready...";
	cout.flush();

	for(int i = 0; i < mapWidth; i++)
	{
		for(int j = 0; j < mapHeight; j++)
		{
			if(map[i][j])
			{
				if(gateClusterMap[i][j] != -1)
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
	bmpFileName << "bmp/" << tMap->getFileName() << "." << distanceDenominator << "_" << wallThreshold << "_" << depthThreshold << "_C_water_level_gates" << ".bmp";
	BitmapWriter::writeBMP(bmpDrawingMap, mapWidth, mapHeight, 2, bmpFileName.str());


	/************BUILD GATE LIST************
	 *
	 */

	vector<Gateway> gates;
	for(int i = 0; i < mapWidth; ++i)
	{
		for(int j = 0; j < mapHeight; ++j)
		{
			if(gateClusterMap[i][j] != -1)
			{
				list<Tile> gateClusterTiles;
				//FloodFill gate cluster with clear and add tiles to gateClusterTiles
				queue<Tile> Q;
				Q.push(Tile(i, j));
				while(!Q.empty())
				{
					Tile node = Q.front();
					Q.pop();
					int x = node.getX();
					int y = node.getY();
					if(gateClusterMap[x][y] != -1)
					{
						int w, e; //w west, e east
						for(w = x; w > 0 && gateClusterMap[w-1][y] != -1; --w){}
						for(e = x; e+1 < mapWidth && gateClusterMap[e+1][y] != -1; ++e){}
						for(int h = w; h <= e; ++h)
						{
							gateClusterTiles.insert(gateClusterTiles.end(), Tile(h, y));
							gateClusterMap[h][y] = -1; //2 is "color" of traversible non-gate tiles

							if(y > 0 && gateClusterMap[h][y-1] != -1) {
								Q.push(Tile(h, y-1)); }
							if(y+1 < mapHeight && gateClusterMap[h][y+1] != -1) {
								Q.push(Tile(h, y+1)); }
						}
					}
				}

				//remove from gateClusterTiles tiles that don't touch wall
				list<Tile>::iterator pTile = gateClusterTiles.begin();
				while(pTile != gateClusterTiles.end())
				{
					int x = pTile->getX();
					int y = pTile->getY();
					if(x > 0 && map[x-1][y]
					&& x+1 < mapWidth && map[x+1][y]
					&& y > 0 && map[x][y-1]
					&& y+1 < mapHeight && map[x][y+1])
					{
						pTile = gateClusterTiles.erase(pTile);
					}
					else
					{
						++pTile;
					}
				}

				//if any tiles left in gateClusterTiles
				if(!gateClusterTiles.empty())
				{
					//if 1 tile left in gateClusterTiles
						//Make new Gateway with same start and end
					if(gateClusterTiles.size() == 1)
					{
						int x = gateClusterTiles.front().getX();
						int y = gateClusterTiles.front().getY();
						gates.insert(gates.end(), Gateway(Tile(x, y), Tile(x, y)));
					}
					//if 2 tiles left in gateClusterTiles
						//Make new Gateway with them
					else if(gateClusterTiles.size() == 2)
					{
						pTile = gateClusterTiles.begin();
						int x1 = pTile->getX();
						int y1 = pTile->getY();
						++pTile;
						int x2 = pTile->getX();
						int y2 = pTile->getY();
						gates.insert(gates.end(), Gateway(Tile(x1, y1), Tile(x2, y2)));
					}
					//if more than 2 tiles left in gateClusterTiles
						//choose the two that have maximum distance between them
						//Make new Gateway with them
					else
					{
						int maxDistSqr = 0;
						Tile start;
						Tile end;
						list<Tile>::iterator pTile1 = gateClusterTiles.begin();
						while(pTile1 != gateClusterTiles.end())
						{
							int x1 = pTile1->getX();
							int y1 = pTile1->getY();
							list<Tile>::iterator pTile2 = pTile1;
							++pTile2;
							while(pTile2 != gateClusterTiles.end())
							{
								int x2 = pTile2->getX();
								int y2 = pTile2->getY();
								int distSqr = (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2);
								if(distSqr > maxDistSqr)
								{
									maxDistSqr = distSqr;
									start.setX(x1);
									start.setY(y1);
									end.setX(x2);
									end.setY(y2);
								}
								++pTile2;
							}
							++pTile1;
						}
						gates.insert(gates.end(), Gateway(start, end));
					}
				}
			}
		}
	}

	cout << "GateList ready...";
	cout.flush();

	/************CLEAN UP************
	 *
	 */

	for(int i = 0; i < mapWidth; i++)
	{
		//delete[] depthMap[i]; Memory was reused
		delete[] zoneClusterMap[i];
		delete[] gateClusterMap[i];
		delete[] bmpDrawingMap[i];
	}
	//delete[] depthMap; Memory was reused
	delete[] zoneClusterMap;
	delete[] gateClusterMap;
	delete[] bmpDrawingMap;
	delete[] tileDepthMap;
/*
	clock_t endTime = clock();
	cout << "Decomposition finished in " << double(endTime - startTime) / CLOCKS_PER_SEC << " seconds" << endl;
*/
	return gates;
}

bool WaterLevelDecomposer::coordsTraversible(int x, int y)
{
	return x >= 0 && x < mapWidth && y >= 0 && y < mapHeight && map[x][y];
}

bool WaterLevelDecomposer::coordsInsideMapAndSomeZone(int x, int y)
{
	return x >= 0 && x < mapWidth && y >= 0 && y < mapHeight && zoneClusterMap[x][y] != -1;
}

bool WaterLevelDecomposer::traversibleNonZoneNonGate(int x, int y)
{
	return map[x][y] && zoneClusterMap[x][y] == -1 && gateClusterMap[x][y] == -1;
}

int WaterLevelDecomposer::octileDistance(int deltaX, int deltaY)
{
	return SINGLE_TILE_DISTANCE * max(deltaX, deltaY) + DIAGONAL_ADDITION * min(deltaX, deltaY);
}

}
