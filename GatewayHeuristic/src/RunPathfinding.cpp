/*
 * AStar.cpp
 *
 *  Created on: Dec 1, 2014
 *      Author: kaha
 */

#include <cmath>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <fstream>

#include <AStar/AStar.h>
#include <AStar/micropather.h>
#include <Pathfinding/PathfindingStateSpace.h>
#include <Pathfinding/PathfindingStateSpaceMP.h>
#include <GatewayPathfinding/GatewayPathfindingStateSpace.h>
#include <GatewayPathfinding/GatewayPathfindingStateSpaceMP.h>
#include <GatewayPathfinding/GateDistanceStateSpace.h>
#include <GatewayDecomposition/WaterLevelDecomposer.h>

using namespace std;

using namespace AStar;
using namespace Pathfinding;
using namespace GatewayDecomposition;

bool loggingSearch;
ofstream searchLog;
ostream *errorLog;
ostream *outputLog;
bool verbose;

void runAStar(StateSpace* stateSpace, int startStateID, int goalStateID, double targetCost, int &out_expanded, int &out_reexpanded)
{
	if(stateSpace->isPossibleState(startStateID) && stateSpace->isPossibleState(goalStateID))
	{
		if(verbose) {
			*outputLog << "Running A* on start: ";
			stateSpace->writeStateToStream(startStateID, outputLog);
			*outputLog << " & goal: ";
			stateSpace->writeStateToStream(goalStateID, outputLog);
		}

		if(loggingSearch) {
			searchLog << "search" << endl;
		}

		AStar::AStar astar(stateSpace, startStateID, goalStateID);
		pair<deque<int>, double> pathPair = astar.findSolution(loggingSearch, &searchLog, out_expanded, out_reexpanded);
		double pathCost = pathPair.second;

		if(verbose) {
			*outputLog << " - Final path - cost: " << pathCost << " - optimal cost: " << targetCost
					<< " - nodes expanded: " << out_expanded << endl;
		}
		if((pathCost - targetCost) > 0.1 || (pathCost - targetCost) < -0.1) {
			*errorLog << "ERROR_MISMATCHED_VALUES - Path cost: "
					<< pathCost << " - optimal cost: " << targetCost
					<< " - start: ";
			stateSpace->writeStateToStream(startStateID, errorLog);
			*errorLog << " - goal: ";
			stateSpace->writeStateToStream(goalStateID, errorLog);
			*errorLog << endl;
		}

		if(loggingSearch) {
			searchLog << "path" << endl;
			deque<int> path = pathPair.first;
			for(deque<int>::iterator tile = path.begin(); tile != path.end(); ++tile)
			{
				if(loggingSearch) {
					stateSpace->writeStateToStream(*tile, &searchLog);
					searchLog << endl;
				}
			}
		}
	}
	else
	{
		if(!stateSpace->isPossibleState(startStateID))
		{
			*errorLog << "ERROR_IMPOSSIBLE_STATE - " << endl;
			stateSpace->writeStateToStream(startStateID, errorLog);
			*errorLog << " is not a valid state in this state space" << endl;
			*outputLog << "ERROR_IMPOSSIBLE_STATE - " << endl;
			stateSpace->writeStateToStream(startStateID, outputLog);
			*outputLog << " is not a valid state in this state space" << endl;
		}
		if(!stateSpace->isPossibleState(goalStateID))
		{
			*errorLog << "ERROR_IMPOSSIBLE_STATE - " << endl;
			stateSpace->writeStateToStream(goalStateID, errorLog);
			*errorLog << " is not a valid state in this state space" << endl;
			*outputLog << "ERROR_IMPOSSIBLE_STATE - " << endl;
			stateSpace->writeStateToStream(goalStateID, outputLog);
			*outputLog << " is not a valid state in this state space" << endl;
		}
	}
}
/*
void runAStar(micropather::Graph *stateSpace, void *startStateID, void *goalStateID, double targetCost, int &out_expanded)
{
	if(verbose) {
		*outputLog << "Running A* on start: ";
		stateSpace->PrintStateInfo(startStateID);
		*outputLog << " & goal: ";
		stateSpace->PrintStateInfo(goalStateID);
	}

	if(loggingSearch) {
		searchLog << "search" << endl;
	}

	micropather::MicroPather pather( stateSpace );
	micropather::MPVector< void* > path;
	float pathCost = 0;
	int result = pather.Solve(startStateID, goalStateID, &path, &pathCost);

	//cout << "pathCost: " << pathCost << endl;

	if(verbose) {
		*outputLog << " - Final path - cost: " << pathCost << " - optimal cost: " << targetCost
				<< " - nodes expanded: " << out_expanded << endl;
	}
	if((pathCost - targetCost) > 0.1 || (pathCost - targetCost) < -0.1) {
		cout << "ERROR_MISMATCHED_VALUES - Path cost: "
				<< pathCost << " - optimal cost: " << targetCost
				<< " - start: ";
		stateSpace->PrintStateInfo(startStateID);
		cout << " - goal: ";
		stateSpace->PrintStateInfo(goalStateID);
		cout << endl;
	}
}
*/
void runScenario(string scenarioFile)
{
	loggingSearch = false;

	ifstream fin;
	string text;
	double value;
	int startX;
	int startY;
	int goalX;
	int goalY;
	double optimalCost;
	string fileName = "noFile";
	TraversibleMap *tMap = 0;
	PathfindingStateSpace *stateSpace = 0;

	fin.open(scenarioFile.c_str());
	fin >> text;
	if(text != "version")
		return;
	fin >> value;
	if(value != 1)
		return;

	fin >> value; //bucket

	while(!fin.eof())
	{
		fin >> text;
		if(text != fileName.c_str())
		{
			fileName = text;
			*outputLog << "Setting up StateSpace " << fileName << endl;
			if(stateSpace != 0){ delete stateSpace; delete tMap; }
			tMap = new TraversibleMap(fileName);
			stateSpace = new PathfindingStateSpace(tMap);
		}

		fin >> value;	//mapWidth
		fin >> value;	//mapHeight
		fin >> startX;
		fin >> startY;
		fin >> goalX;
		fin >> goalY;
		fin >> optimalCost;
		fin >> value; //bucket

		int out_expanded = 0;
		int out_reexpanded = 0;

		runAStar(stateSpace, stateSpace->getStateID(startX, startY),
				stateSpace->getStateID(goalX, goalY), optimalCost, out_expanded, out_reexpanded);
	}
	if(stateSpace != 0){ delete stateSpace; delete tMap; }
}

void runSingleSearch(PathfindingStateSpace *stateSpace, int startX, int startY, int goalX, int goalY, double targetCost, string logFileName)
{
	loggingSearch = true;
	//*outputLog << "Logging to file: " << logFileName << endl;
	searchLog.open(logFileName.c_str());

	int out_expanded = 0;
	int out_reexpanded = 0;

	runAStar(stateSpace, stateSpace->getStateID(startX, startY),
								stateSpace->getStateID(goalX, goalY), targetCost, out_expanded, out_reexpanded);

	searchLog.close();
}

/*
void runSingleSearch(PathfindingStateSpaceMP *stateSpace, int startX, int startY, int goalX, int goalY, double targetCost)
{
	loggingSearch = true;
	// *outputLog << "Logging to file: " << logFileName << endl;
	//searchLog.open(logFileName.c_str());

	int out_expanded = 0;

	runAStar(stateSpace, (void*)stateSpace->getStateID(startX, startY),
								(void*)stateSpace->getStateID(goalX, goalY), targetCost, out_expanded);

	//searchLog.close();
}
*/

/*
void decomposeSingleMapMultiThreshold(string mapFile)
{
	TraversibleMap *tMap = new TraversibleMap(mapFile);

	WaterLevelDecomposer decomposer;

	int distanceDenominator = 1;
	while(distanceDenominator <= 64)
	{
		int wallThreshold = 1;
		while(wallThreshold <= 512)
		{
			cout << distanceDenominator << ", " << wallThreshold << " - ";
			cout.flush();
			GatewayMap gatewayMap = decomposer.decompose(tMap, distanceDenominator, wallThreshold, 1);

			wallThreshold *= 2;
		}

		distanceDenominator *= 2;
	}

	delete tMap;
}

void decomposeSingleMapDynamicThreshold(string mapFile)
{
	TraversibleMap *tMap = new TraversibleMap(mapFile);

	WaterLevelDecomposer decomposer;

	GatewayMap gatewayMap = decomposer.decompose(tMap, 0, 0, 1);
	//decomposer.decompose(tMap, 0, 128, 1);
	//decomposer.decompose(tMap, 64, 0, 1);
	//decomposer.decompose(tMap, 64, 128, 1);

	gatewayMap.printAll(&cout);
	gatewayMap.calculateDistances();

	delete tMap;
}
*/
int runScenario(string scenarioFile, PathfindingStateSpace *stateSpace, int firstSearch, int numberOfSearches)
{
	cout << "running scenario: " << scenarioFile << endl;
	*errorLog << "running scenario: " << scenarioFile << endl;

	loggingSearch = false;

	ifstream fin;
	string text;
	double value;
	int startX;
	int startY;
	int goalX;
	int goalY;
	double optimalCost;
	string fileName = "noFile";

	fin.open(scenarioFile.c_str());
	fin >> text;
	if(text != "version")
		return 0;
	fin >> value;
	if(value != 1)
		return 0;

	fin >> value; //bucket

	int currentEntry = 0;
	int lastEntry = firstSearch + numberOfSearches;

	int sum_expanded = 0;
	int sum_reexpanded = 0;

	double currentSearchNum = 0;
	double averagePathLength = 0;
	double averageNodesExpanded = 0;
	double averageReExpanded = 0;
	double averageTime = 0;

	while(!fin.eof())
	{
		fin >> text;

		fin >> value;	//mapWidth
		fin >> value;	//mapHeight
		fin >> startX;
		fin >> startY;
		fin >> goalX;
		fin >> goalY;
		fin >> optimalCost;
		fin >> value; //bucket

		if(currentEntry < firstSearch) {
			currentEntry++;
			continue;
		}

		int out_expanded = 0;
		int out_reexpanded = 0;

		clock_t startTime = clock();

		runAStar(stateSpace, stateSpace->getStateID(startX, startY),
				stateSpace->getStateID(goalX, goalY), optimalCost, out_expanded, out_reexpanded);

		double time = double(clock() - startTime) / CLOCKS_PER_SEC;

		sum_expanded += out_expanded;
		sum_reexpanded += out_reexpanded;

		averagePathLength *= currentSearchNum;
		averageNodesExpanded *= currentSearchNum;
		averageReExpanded *= currentSearchNum;
		averageTime *= currentSearchNum;

		currentSearchNum += 1.0;

		averagePathLength = (averagePathLength + optimalCost) / currentSearchNum;
		averageNodesExpanded = (averageNodesExpanded + out_expanded) / currentSearchNum;
		averageReExpanded = (averageReExpanded + out_reexpanded) / currentSearchNum;
		averageTime = (averageTime + time) / currentSearchNum;

		currentEntry++;

/*
		if(currentEntry % 10 == 0)
		{
			clock_t currentTime = clock();
			*outputLog << "CurrentTime: " << double(currentTime - startTime) / CLOCKS_PER_SEC
					<< " - Nodes expanded: " << sum_expanded
					<< " - (" << startX << ", " << startY << ") - (" << goalX << ", " << goalY << ")" << endl;
			// *outputLog << "" << double(currentTime - startTime) / CLOCKS_PER_SEC
			//		<< "\t" << sum_expanded << endl;
			startTime = currentTime;
			sum_expanded = 0;
		}
*/
		if(currentEntry == lastEntry)
		{
			break;
		}
	}
	*outputLog << " " << averagePathLength << " & " << averageTime << " & "
						<< averageNodesExpanded << " & " << averageReExpanded << " & "
						<< (int)currentSearchNum;

	return (int)currentSearchNum;
}

/*
int runScenario(string scenarioFile, PathfindingStateSpaceMP *stateSpace, int firstSearch, int numberOfSearches)
{
	cout << "running scenario: " << scenarioFile << endl;
	*errorLog << "running scenario: " << scenarioFile << endl;

	loggingSearch = false;

	ifstream fin;
	string text;
	double value;
	int startX;
	int startY;
	int goalX;
	int goalY;
	double optimalCost;
	string fileName = "noFile";

	fin.open(scenarioFile.c_str());
	fin >> text;
	if(text != "version")
		return 0;
	fin >> value;
	if(value != 1)
		return 0;

	fin >> value; //bucket

	int currentEntry = 0;
	int lastEntry = firstSearch + numberOfSearches;

	int sum_expanded = 0;

	double currentSearchNum = 0;
	double averagePathLength = 0;
	double averageNodesExpanded = 0;
	double averageTime = 0;

	while(!fin.eof())
	{
		fin >> text;

		fin >> value;	//mapWidth
		fin >> value;	//mapHeight
		fin >> startX;
		fin >> startY;
		fin >> goalX;
		fin >> goalY;
		fin >> optimalCost;
		fin >> value; //bucket

		if(currentEntry < firstSearch) {
			currentEntry++;
			continue;
		}

		int out_expanded = 0;

		clock_t startTime = clock();

		runAStar(stateSpace, (void*)stateSpace->getStateID(startX, startY),
				(void*)stateSpace->getStateID(goalX, goalY), optimalCost, out_expanded);

		double time = double(clock() - startTime) / CLOCKS_PER_SEC;

		sum_expanded += out_expanded;

		averagePathLength *= currentSearchNum;
		averageNodesExpanded *= currentSearchNum;
		averageTime *= currentSearchNum;

		currentSearchNum += 1.0;

		averagePathLength = (averagePathLength + optimalCost) / currentSearchNum;
		averageNodesExpanded = (averageNodesExpanded + out_expanded) / currentSearchNum;
		averageTime = (averageTime + time) / currentSearchNum;

		currentEntry++;

//
//		if(currentEntry % 10 == 0)
//		{
//			clock_t currentTime = clock();
//			*outputLog << "CurrentTime: " << double(currentTime - startTime) / CLOCKS_PER_SEC
//					<< " - Nodes expanded: " << sum_expanded
//					<< " - (" << startX << ", " << startY << ") - (" << goalX << ", " << goalY << ")" << endl;
//			// *outputLog << "" << double(currentTime - startTime) / CLOCKS_PER_SEC
//			//		<< "\t" << sum_expanded << endl;
//			startTime = currentTime;
//			sum_expanded = 0;
//		}

		if(currentEntry == lastEntry)
		{
			break;
		}
	}
	*outputLog << " " << averagePathLength << " & " << averageTime << " & "
						<< averageNodesExpanded << " & " << (int)currentSearchNum;

	return (int)currentSearchNum;
}
*/

void decomposeSingleBMPOnly(string mapName, string gameName)
{
	loggingSearch = true;
	verbose = true;
	ofstream *errorFile = new ofstream();
	errorFile->open("logs/_error.log", ios::out|ios::app);
	errorLog = errorFile;

	TraversibleMap *tMap;

	stringstream mapFile;
	mapFile << "maps/" << gameName << "/" << mapName << ".map";
	tMap = new TraversibleMap(mapFile.str());

	clock_t startTime = clock();

	WaterLevelDecomposer decomposer;

/*	vector<Gateway> gates = decomposer.decompose(tMap, 1, 1, 1);
	gates = decomposer.decompose(tMap, 64, 1, 1);
	gates = decomposer.decompose(tMap, 1, 128, 1);
	gates = decomposer.decompose(tMap, 64, 128, 1);
	gates = decomposer.decompose(tMap, 0, 0, 1);
*/
	vector<Gateway> gates = decomposer.decompose(tMap, 1, 1, 1);

	gates = decomposer.decompose(tMap, 0, 1, 1);
	gates = decomposer.decompose(tMap, 1, 0, 1);
	gates = decomposer.decompose(tMap, 0, 0, 1);


	int gateCount = gates.size();
	Gateway *p_gates = new Gateway[gateCount];

	for(int i = 0; i < gateCount; i++)
	{
		p_gates[i].setStart(gates[i].getStart());
		p_gates[i].setEnd(gates[i].getEnd());
	}
	GatewayMap gatewayMap(tMap);
	gatewayMap.setGates(p_gates, gateCount);

	gatewayMap.calculateZones();

	clock_t endTime = clock();

	cout << mapName << " & " << double(endTime - startTime) / CLOCKS_PER_SEC
			<< " & " << gatewayMap.getTraversibleMap()->getWidth() << "x" << gatewayMap.getTraversibleMap()->getHeight()
			<< " & " << gatewayMap.getGateCount()
			<< " & " << gatewayMap.getZoneCount() << " \\\\ " << endl;
	//delete tMap;

	errorFile->close();
}

void decomposeMain(int begin, int end)
{
	loggingSearch = true;
	verbose = true;
	ofstream *errorFile = new ofstream();
	errorFile->open("logs/_error.log", ios::out|ios::app);
	errorLog = errorFile;

	for(int i = begin; i < end; ++i)
	{
		TraversibleMap *tMap;
		string mapName, gameName;

		switch(i) {
		case 0: mapName = "AcrosstheCape"; gameName = "sc1"; break;
		case 1: mapName = "Aftershock"; gameName = "sc1"; break;
		case 2: mapName = "Archipelago"; gameName = "sc1"; break;
		case 3: mapName = "ArcticStation"; gameName = "sc1"; break;
		case 4: mapName = "Aurora"; gameName = "sc1"; break;
		case 5: mapName = "Backwoods"; gameName = "sc1"; break;
		case 6: mapName = "BigGameHunters"; gameName = "sc1"; break;
		case 7: mapName = "BlackLotus"; gameName = "sc1"; break;
		case 8: mapName = "BlastFurnace"; gameName = "sc1"; break;
		case 9: mapName = "BrokenSteppes"; gameName = "sc1"; break;
		case 10: mapName = "Brushfire"; gameName = "sc1"; break;
		case 11: mapName = "Caldera"; gameName = "sc1"; break;
		case 12: mapName = "CatwalkAlley"; gameName = "sc1"; break;
		case 13: mapName = "Cauldron"; gameName = "sc1"; break;
		case 14: mapName = "CrashSites"; gameName = "sc1"; break;
		case 15: mapName = "CrescentMoon"; gameName = "sc1"; break;
		case 16: mapName = "Crossroads"; gameName = "sc1"; break;
		case 17: mapName = "DarkContinent"; gameName = "sc1"; break;
		case 18: mapName = "Desolation"; gameName = "sc1"; break;
		case 19: mapName = "EbonLakes"; gameName = "sc1"; break;
		case 20: mapName = "Elderlands"; gameName = "sc1"; break;
		case 21: mapName = "Enigma"; gameName = "sc1"; break;
		case 22: mapName = "Entanglement"; gameName = "sc1"; break;
		case 23: mapName = "Eruption"; gameName = "sc1"; break;
		case 24: mapName = "Expedition"; gameName = "sc1"; break;
		case 25: mapName = "FireWalker"; gameName = "sc1"; break;
		case 26: mapName = "FloodedPlains"; gameName = "sc1"; break;
		case 27: mapName = "GhostTown"; gameName = "sc1"; break;
		case 28: mapName = "GladiatorPits"; gameName = "sc1"; break;
		case 29: mapName = "GreenerPastures"; gameName = "sc1"; break;
		case 30: mapName = "Hellfire"; gameName = "sc1"; break;
		case 31: mapName = "AR0011SR"; gameName = "bg2"; break;
		case 32: mapName = "AR0012SR"; gameName = "bg2"; break;
		case 33: mapName = "AR0016SR"; gameName = "bg2"; break;
		case 34: mapName = "AR0017SR"; gameName = "bg2"; break;
		case 35: mapName = "AR0018SR"; gameName = "bg2"; break;
		case 36: mapName = "AR0020SR"; gameName = "bg2"; break;
		case 37: mapName = "AR0041SR"; gameName = "bg2"; break;
		case 38: mapName = "AR0042SR"; gameName = "bg2"; break;
		case 39: mapName = "AR0043SR"; gameName = "bg2"; break;
		case 40: mapName = "AR0044SR"; gameName = "bg2"; break;
		case 41: mapName = "AR0045SR"; gameName = "bg2"; break;
		case 42: mapName = "AR0046SR"; gameName = "bg2"; break;
		case 43: mapName = "AR0070SR"; gameName = "bg2"; break;
		case 44: mapName = "AR0071SR"; gameName = "bg2"; break;
		case 45: mapName = "AR0072SR"; gameName = "bg2"; break;
		case 46: mapName = "AR0201SR"; gameName = "bg2"; break;
		case 47: mapName = "AR0202SR"; gameName = "bg2"; break;
		case 48: mapName = "AR0203SR"; gameName = "bg2"; break;
		case 49: mapName = "AR0205SR"; gameName = "bg2"; break; //original
		case 50: mapName = "AR0206SR"; gameName = "bg2"; break;
		case 51: mapName = "AR0300SR"; gameName = "bg2"; break;
		case 52: mapName = "AR0302SR"; gameName = "bg2"; break;
		case 53: mapName = "AR0304SR"; gameName = "bg2"; break;
		case 54: mapName = "AR0306SR"; gameName = "bg2"; break;
		case 55: mapName = "AR0308SR"; gameName = "bg2"; break;
		case 56: mapName = "AR0400SR"; gameName = "bg2"; break;
		case 57: mapName = "AR0404SR"; gameName = "bg2"; break;
		case 58: mapName = "AR0405SR"; gameName = "bg2"; break;
		case 59: mapName = "AR0406SR"; gameName = "bg2"; break; //original
		case 60: mapName = "AR0412SR"; gameName = "bg2"; break;
		case 61: mapName = "AR0418SR"; gameName = "bg2"; break;
		case 62: mapName = "AR0500SR"; gameName = "bg2"; break;
		case 63: mapName = "AR0526SR"; gameName = "bg2"; break;
		case 64: mapName = "AR0600SR"; gameName = "bg2"; break;
		case 65: mapName = "AR0602SR"; gameName = "bg2"; break;
		case 66: mapName = "AR0605SR"; gameName = "bg2"; break;
		case 67: mapName = "AR0700SR"; gameName = "bg2"; break;
		case 68: mapName = "AR0711SR"; gameName = "bg2"; break;
		case 69: mapName = "AcrosstheCape"; gameName = "sc1_low_res"; break;
		case 70: mapName = "ArcticStation"; gameName = "sc1_low_res"; break;
		case 71: mapName = "Backwoods"; gameName = "sc1_low_res"; break;
		case 72: mapName = "BigGameHunters"; gameName = "sc1_low_res"; break;
		case 73: mapName = "BlackLotus"; gameName = "sc1_low_res"; break;
		case 74: mapName = "BlastFurnace"; gameName = "sc1_low_res"; break;
		case 75: mapName = "BrokenSteppes"; gameName = "sc1_low_res"; break;
		case 76: mapName = "Brushfire"; gameName = "sc1_low_res"; break;
		case 77: mapName = "CatwalkAlley"; gameName = "sc1_low_res"; break;
		case 78: mapName = "Cauldron"; gameName = "sc1_low_res"; break;
		case 79: mapName = "Crossroads"; gameName = "sc1_low_res"; break;
		case 80: mapName = "DarkContinent"; gameName = "sc1_low_res"; break;
		case 81: mapName = "Elderlands"; gameName = "sc1_low_res"; break;
		case 82: mapName = "Enigma"; gameName = "sc1_low_res"; break;
		case 83: mapName = "FireWalker"; gameName = "sc1_low_res"; break;
		case 84: mapName = "FloodedPlains"; gameName = "sc1_low_res"; break;
		case 85: mapName = "GladiatorPits"; gameName = "sc1_low_res"; break;
		}

		stringstream mapFile;
		mapFile << "maps/" << gameName << "/" << mapName << ".map";
		tMap = new TraversibleMap(mapFile.str());

		clock_t time = clock();

		WaterLevelDecomposer decomposer;
		vector<Gateway> gates = decomposer.decompose(tMap, 0, 0, 1);

		int gateCount = gates.size();
		Gateway *p_gates = new Gateway[gateCount];

		for(int i = 0; i < gateCount; i++)
		{
			p_gates[i].setStart(gates[i].getStart());
			p_gates[i].setEnd(gates[i].getEnd());
		}
		GatewayMap gatewayMap(tMap);
		gatewayMap.setGates(p_gates, gateCount);

		gatewayMap.calculateZones();

		time = clock() - time;

		*errorFile << mapFile.str() << " decomposed in "
				<< (double)time / (double)CLOCKS_PER_SEC << " seconds"
				<< " with " << gatewayMap.getGateCount() << " gates" << endl;


		stringstream mapDataFile;
		mapDataFile << "mapdata/" << gameName << "/" << mapName << ".map.dat";
		gatewayMap.writeMapData(mapDataFile.str());

		//delete tMap;
	}

	errorFile->close();
}

void calculateDistancesMain(int begin, int end)
{
	loggingSearch = true;
	verbose = true;
	ofstream *errorFile = new ofstream();
	errorFile->open("logs/_error.log", ios::out|ios::app);
	errorLog = errorFile;

	for(int i = begin; i < end; ++i)
	{
		GatewayMap *gatewayMap;
		string mapName, gameName;

		switch(i) {
		case 0: mapName = "AcrosstheCape"; gameName = "sc1"; break;
		case 1: mapName = "Aftershock"; gameName = "sc1"; break;
		case 2: mapName = "Archipelago"; gameName = "sc1"; break;
		case 3: mapName = "ArcticStation"; gameName = "sc1"; break;
		case 4: mapName = "Aurora"; gameName = "sc1"; break;
		case 5: mapName = "Backwoods"; gameName = "sc1"; break;
		case 6: mapName = "BigGameHunters"; gameName = "sc1"; break;
		case 7: mapName = "BlackLotus"; gameName = "sc1"; break;
		case 8: mapName = "BlastFurnace"; gameName = "sc1"; break;
		case 9: mapName = "BrokenSteppes"; gameName = "sc1"; break;
		case 10: mapName = "Brushfire"; gameName = "sc1"; break;
		case 11: mapName = "Caldera"; gameName = "sc1"; break;
		case 12: mapName = "CatwalkAlley"; gameName = "sc1"; break;
		case 13: mapName = "Cauldron"; gameName = "sc1"; break;
		case 14: mapName = "CrashSites"; gameName = "sc1"; break;
		case 15: mapName = "CrescentMoon"; gameName = "sc1"; break;
		case 16: mapName = "Crossroads"; gameName = "sc1"; break;
		case 17: mapName = "DarkContinent"; gameName = "sc1"; break;
		case 18: mapName = "Desolation"; gameName = "sc1"; break;
		case 19: mapName = "EbonLakes"; gameName = "sc1"; break;
		case 20: mapName = "Elderlands"; gameName = "sc1"; break;
		case 21: mapName = "Enigma"; gameName = "sc1"; break;
		case 22: mapName = "Entanglement"; gameName = "sc1"; break;
		case 23: mapName = "Eruption"; gameName = "sc1"; break;
		case 24: mapName = "Expedition"; gameName = "sc1"; break;
		case 25: mapName = "FireWalker"; gameName = "sc1"; break;
		case 26: mapName = "FloodedPlains"; gameName = "sc1"; break;
		case 27: mapName = "GhostTown"; gameName = "sc1"; break;
		case 28: mapName = "GladiatorPits"; gameName = "sc1"; break;
		case 29: mapName = "GreenerPastures"; gameName = "sc1"; break;
		case 30: mapName = "Hellfire"; gameName = "sc1"; break;
		case 31: mapName = "AR0011SR"; gameName = "bg2"; break;
		case 32: mapName = "AR0012SR"; gameName = "bg2"; break;
		case 33: mapName = "AR0016SR"; gameName = "bg2"; break;
		case 34: mapName = "AR0017SR"; gameName = "bg2"; break;
		case 35: mapName = "AR0018SR"; gameName = "bg2"; break;
		case 36: mapName = "AR0020SR"; gameName = "bg2"; break;
		case 37: mapName = "AR0041SR"; gameName = "bg2"; break;
		case 38: mapName = "AR0042SR"; gameName = "bg2"; break;
		case 39: mapName = "AR0043SR"; gameName = "bg2"; break;
		case 40: mapName = "AR0044SR"; gameName = "bg2"; break;
		case 41: mapName = "AR0045SR"; gameName = "bg2"; break;
		case 42: mapName = "AR0046SR"; gameName = "bg2"; break;
		case 43: mapName = "AR0070SR"; gameName = "bg2"; break;
		case 44: mapName = "AR0071SR"; gameName = "bg2"; break;
		case 45: mapName = "AR0072SR"; gameName = "bg2"; break;
		case 46: mapName = "AR0201SR"; gameName = "bg2"; break;
		case 47: mapName = "AR0202SR"; gameName = "bg2"; break;
		case 48: mapName = "AR0203SR"; gameName = "bg2"; break;
		case 49: mapName = "AR0205SR"; gameName = "bg2"; break; //original
		case 50: mapName = "AR0206SR"; gameName = "bg2"; break;
		case 51: mapName = "AR0300SR"; gameName = "bg2"; break;
		case 52: mapName = "AR0302SR"; gameName = "bg2"; break;
		case 53: mapName = "AR0304SR"; gameName = "bg2"; break;
		case 54: mapName = "AR0306SR"; gameName = "bg2"; break;
		case 55: mapName = "AR0308SR"; gameName = "bg2"; break;
		case 56: mapName = "AR0400SR"; gameName = "bg2"; break;
		case 57: mapName = "AR0404SR"; gameName = "bg2"; break;
		case 58: mapName = "AR0405SR"; gameName = "bg2"; break;
		case 59: mapName = "AR0406SR"; gameName = "bg2"; break; //original
		case 60: mapName = "AR0412SR"; gameName = "bg2"; break;
		case 61: mapName = "AR0418SR"; gameName = "bg2"; break;
		case 62: mapName = "AR0500SR"; gameName = "bg2"; break;
		case 63: mapName = "AR0526SR"; gameName = "bg2"; break;
		case 64: mapName = "AR0600SR"; gameName = "bg2"; break;
		case 65: mapName = "AR0602SR"; gameName = "bg2"; break;
		case 66: mapName = "AR0605SR"; gameName = "bg2"; break;
		case 67: mapName = "AR0700SR"; gameName = "bg2"; break;
		case 68: mapName = "AR0711SR"; gameName = "bg2"; break;
		}

		stringstream mapDataFile;
		mapDataFile << "mapdata/" << gameName << "/" << mapName << ".map.dat";
		gatewayMap = new GatewayMap(mapDataFile.str());

		gatewayMap->printAll(&cout);

		gatewayMap->calculateDistances();

		gatewayMap->printAll(&cout);

		stringstream distanceDataFile;
		distanceDataFile << "mapdata/" << gameName << "/" << mapName << ".dist.dat";
		gatewayMap->writeDistanceData(distanceDataFile.str());

		delete gatewayMap;
	}

	errorFile->close();
}

void bulkSearchMain(bool doOctileSearch, bool doGatewaySearch, int mapBegin, int mapEnd, int searchBegin, int searchCount, bool topTenPercent = false)
{
	loggingSearch = true;
	verbose = false;
	ofstream *errorFile = new ofstream();
	errorFile->open("logs/_error.log", ios::out|ios::app);
	errorLog = errorFile;

	for(int i = mapBegin; i < mapEnd; ++i)
	{
		GatewayMap *gatewayMap;
		string mapName, gameName;

		switch(i) {
		case 0: mapName = "AcrosstheCape"; gameName = "sc1"; break;
		case 1: mapName = "Aftershock"; gameName = "sc1"; break;
		case 2: mapName = "Archipelago"; gameName = "sc1"; break;
		case 3: mapName = "ArcticStation"; gameName = "sc1"; break;
		case 4: mapName = "Aurora"; gameName = "sc1"; break;
		case 5: mapName = "Backwoods"; gameName = "sc1"; break;
		case 6: mapName = "BigGameHunters"; gameName = "sc1"; break;
		case 7: mapName = "BlackLotus"; gameName = "sc1"; break;
		case 8: mapName = "BlastFurnace"; gameName = "sc1"; break;
		case 9: mapName = "BrokenSteppes"; gameName = "sc1"; break;
		case 10: mapName = "Brushfire"; gameName = "sc1"; break;
		case 11: mapName = "Caldera"; gameName = "sc1"; break;
		case 12: mapName = "CatwalkAlley"; gameName = "sc1"; break;
		case 13: mapName = "Cauldron"; gameName = "sc1"; break;
		case 14: mapName = "CrashSites"; gameName = "sc1"; break;
		case 15: mapName = "CrescentMoon"; gameName = "sc1"; break;
		case 16: mapName = "Crossroads"; gameName = "sc1"; break;
		case 17: mapName = "DarkContinent"; gameName = "sc1"; break;
		case 18: mapName = "Desolation"; gameName = "sc1"; break;
		case 19: mapName = "EbonLakes"; gameName = "sc1"; break;
		case 20: mapName = "Elderlands"; gameName = "sc1"; break;
		case 21: mapName = "Enigma"; gameName = "sc1"; break;
		case 22: mapName = "Entanglement"; gameName = "sc1"; break;
		case 23: mapName = "Eruption"; gameName = "sc1"; break;
		case 24: mapName = "Expedition"; gameName = "sc1"; break;
		case 25: mapName = "FireWalker"; gameName = "sc1"; break;
		case 26: mapName = "FloodedPlains"; gameName = "sc1"; break;
		case 27: mapName = "GhostTown"; gameName = "sc1"; break;
		case 28: mapName = "GladiatorPits"; gameName = "sc1"; break;
		case 29: mapName = "GreenerPastures"; gameName = "sc1"; break;
		case 30: mapName = "Hellfire"; gameName = "sc1"; break;
		case 31: mapName = "AR0011SR"; gameName = "bg2"; break;
		case 32: mapName = "AR0012SR"; gameName = "bg2"; break;
		case 33: mapName = "AR0016SR"; gameName = "bg2"; break;
		case 34: mapName = "AR0017SR"; gameName = "bg2"; break;
		case 35: mapName = "AR0018SR"; gameName = "bg2"; break;
		case 36: mapName = "AR0020SR"; gameName = "bg2"; break;
		case 37: mapName = "AR0041SR"; gameName = "bg2"; break;
		case 38: mapName = "AR0042SR"; gameName = "bg2"; break;
		case 39: mapName = "AR0043SR"; gameName = "bg2"; break;
		case 40: mapName = "AR0044SR"; gameName = "bg2"; break;
		case 41: mapName = "AR0045SR"; gameName = "bg2"; break;
		case 42: mapName = "AR0046SR"; gameName = "bg2"; break;
		case 43: mapName = "AR0070SR"; gameName = "bg2"; break;
		case 44: mapName = "AR0071SR"; gameName = "bg2"; break;
		case 45: mapName = "AR0072SR"; gameName = "bg2"; break;
		case 46: mapName = "AR0201SR"; gameName = "bg2"; break;
		case 47: mapName = "AR0202SR"; gameName = "bg2"; break;
		case 48: mapName = "AR0203SR"; gameName = "bg2"; break;
		case 49: mapName = "AR0205SR"; gameName = "bg2"; break; //original
		case 50: mapName = "AR0206SR"; gameName = "bg2"; break;
		case 51: mapName = "AR0300SR"; gameName = "bg2"; break;
		case 52: mapName = "AR0302SR"; gameName = "bg2"; break;
		case 53: mapName = "AR0304SR"; gameName = "bg2"; break;
		case 54: mapName = "AR0306SR"; gameName = "bg2"; break;
		case 55: mapName = "AR0308SR"; gameName = "bg2"; break;
		case 56: mapName = "AR0400SR"; gameName = "bg2"; break;
		case 57: mapName = "AR0404SR"; gameName = "bg2"; break;
		case 58: mapName = "AR0405SR"; gameName = "bg2"; break;
		case 59: mapName = "AR0406SR"; gameName = "bg2"; break; //original
		case 60: mapName = "AR0412SR"; gameName = "bg2"; break;
		case 61: mapName = "AR0418SR"; gameName = "bg2"; break;
		case 62: mapName = "AR0500SR"; gameName = "bg2"; break;
		case 63: mapName = "AR0526SR"; gameName = "bg2"; break;
		case 64: mapName = "AR0600SR"; gameName = "bg2"; break;
		case 65: mapName = "AR0602SR"; gameName = "bg2"; break;
		case 66: mapName = "AR0605SR"; gameName = "bg2"; break;
		case 67: mapName = "AR0700SR"; gameName = "bg2"; break;
		case 68: mapName = "AR0711SR"; gameName = "bg2"; break;
		}


		stringstream mapDataFile;
		mapDataFile << "mapdata/" << gameName << "/" << mapName << ".map.dat";
		stringstream distanceDataFile;
		distanceDataFile << "mapdata/" << gameName << "/" << mapName << ".dist.dat";
		stringstream scenario;
		scenario << "scenarios/" << gameName << "/" << mapName << ".map.scen";
		stringstream searchLogString;
		//searchLogString << "logs/Scenario_" << gameName << "_" << mapName << ".log";
		searchLogString << "logs/Results.log";

		gatewayMap = new GatewayMap(mapDataFile.str(), distanceDataFile.str());

		GatewayPathfindingStateSpace *stateSpaceGateways;
		PathfindingStateSpace *stateSpaceOctile;

		if(doGatewaySearch) {
			stateSpaceGateways = new GatewayPathfindingStateSpace(gatewayMap);
			*errorLog << endl;
		}
		if(doOctileSearch) {
			stateSpaceOctile = new PathfindingStateSpace(gatewayMap->getTraversibleMap());
			*errorLog << endl;
		}


		searchLog.open(searchLogString.str().c_str(), ios::out|ios::app);
		outputLog = &searchLog;

		int trueSearchCount;

		if(doOctileSearch) {
			*outputLog << mapName << " (octile) & ";
			trueSearchCount = runScenario(scenario.str(), stateSpaceOctile, searchBegin, searchCount);
			*outputLog << " \\\\" << endl;
		}

		if(doGatewaySearch) {
			*outputLog << mapName << " (gateway) & ";
			trueSearchCount = runScenario(scenario.str(), stateSpaceGateways, searchBegin, searchCount);
			*outputLog << " \\\\" << endl;
		}

		if(topTenPercent)
		{
			if(doOctileSearch) {
				*outputLog << mapName << " (octile - top 10 \\%) & ";
				runScenario(scenario.str(), stateSpaceOctile, searchBegin + 9 * (trueSearchCount / 10), trueSearchCount / 10);
				*outputLog << " \\\\" << endl;
			}

			if(doGatewaySearch) {
				*outputLog << mapName << " (gateway - top 10 \\%) & ";
				runScenario(scenario.str(), stateSpaceGateways, searchBegin + 9 * (trueSearchCount / 10), trueSearchCount / 10);
				*outputLog << " \\\\" << endl;
			}

		}

		searchLog.close();
		if(doOctileSearch) {
			delete stateSpaceOctile;
		}
		if(doGatewaySearch) {
			delete stateSpaceGateways;
		}

		*errorLog << endl;

		delete gatewayMap;
	}

	errorFile->close();
}

void singleSearchMain()
{
	loggingSearch = false;

	outputLog = &cout;
	verbose = false;

	ofstream *errorFile = new ofstream();
	errorFile->open("logs/_error.log", ios::out|ios::app);
	errorLog = errorFile;

	//GatewayMap *gatewayMap = new GatewayMap("mapdata/sc1/Backwoods.map.dat");
	//GatewayMap *gatewayMap = new GatewayMap("mapdata/sc1/Aurora.map.dat", "mapdata/sc1/Aurora.dist.dat");
	GatewayMap *gatewayMap = new GatewayMap("mapdata/sc1/CrashSites.map.dat", "mapdata/sc1/CrashSites.dist.dat");

	GatewayPathfindingStateSpace *stateSpaceGateways = new GatewayPathfindingStateSpace(gatewayMap);

	//stateSpaceGateways->evaluate(569204, 578426);

	//runSingleSearch(stateSpaceGateways, 14, 48, 349, 359, 593.63, "logs/Backwoods_FullSearch_gateway.log");
	//runSingleSearch(stateSpaceGateways, 439, 490, 463, 454, 192.024, "logs/AcrosstheCape_short_gateway.log");
	//runSingleSearch(stateSpaceGateways, 747, 269, 44, 569, 921.683, "logs/AcrosstheCape_long_gateway.log");
	//runSingleSearch(stateSpaceGateways, 690, 310, 8, 685, 1179.8, "logs/AcrosstheCape_longest_gateway.log");
	//runSingleSearch(stateSpaceGateways, 341, 265, 396, 148, 0.01, "logs/AcrosstheCape_trivial_gateway.log");
	//runSingleSearch(stateSpaceGateways, 563, 178, 361, 722, 0.01, "logs/Aurora_long_gateway.log");
	runSingleSearch(stateSpaceGateways, 520, 532, 122, 2, 807.117, "logs/CrashSites_long1_gateway.log");
	runSingleSearch(stateSpaceGateways, 696, 608, 252, 248, 804.548, "logs/CrashSites_long2_gateway.log");


	PathfindingStateSpace *stateSpaceOctile = new PathfindingStateSpace(gatewayMap->getTraversibleMap());
	//runSingleSearch(stateSpaceOctile, 14, 48, 349, 359, 593.63, "logs/Backwoods_FullSearch_octile.log");
	//runSingleSearch(stateSpaceOctile, 439, 490, 463, 454, 192.024, "logs/AcrosstheCape_short_octile.log");
	//runSingleSearch(stateSpaceOctile, 747, 269, 44, 569, 921.683, "logs/AcrosstheCape_long_octile.log");
	//runSingleSearch(stateSpaceOctile, 690, 310, 8, 685, 1179.8, "logs/AcrosstheCape_longest_octile.log");
	//runSingleSearch(stateSpaceOctile, 341, 265, 396, 148, 0.01, "logs/AcrosstheCape_trivial_octile.log");
	//runSingleSearch(stateSpaceOctile, 563, 178, 361, 722, 0.01, "logs/Aurora_long_octile.log");
	runSingleSearch(stateSpaceOctile, 520, 532, 122, 2, 807.117, "logs/CrashSites_long1_octile.log");
	runSingleSearch(stateSpaceOctile, 696, 608, 252, 248, 804.548, "logs/CrashSites_long2_octile.log");

	//PathfindingStateSpace *stateSpaceBreadthFirst = new GateDistanceStateSpace(gatewayMap);
	//runSingleSearch(stateSpaceBreadthFirst, 14, 48, 349, 359, 593.63, "logs/Backwoods_FullSearch_BreadthFirst.log");
	//runSingleSearch(stateSpaceBreadthFirst, 439, 490, 463, 454, 192.024, "logs/AcrosstheCape_short_BreadthFirst.log");
	//runSingleSearch(stateSpaceBreadthFirst, 747, 269, 44, 569, 921.683, "logs/AcrosstheCape_long_BreadthFirst.log");
	//runSingleSearch(stateSpaceBreadthFirst, 690, 310, 8, 685, 1179.8, "logs/AcrosstheCape_longest_BreadthFirst.log");

	//stringstream logFileName;
	//size_t posAfterSlash = mapFile.find_last_of("/") + 1;
	//size_t lengthToDot = mapFile.find_last_of(".") - posAfterSlash;
	//logFileName << "logs/" << mapFile.substr(posAfterSlash, lengthToDot) << "_" << startX << "-" << startY << "-" << goalX << "-" << goalY << ".log";

	//runSingleSearch("maps/mazes/maze512-8-0.map", 65, 326, 65, 326, 0.0);
	//runSingleSearch("maps/mazes/maze512-8-0.map", 65, 326, 407, 13, 2100.16);
	//runSingleSearch("maps/mazes/maze512-8-0.map", 41, 447, 289, 381, 2101.82);
	//runSingleSearch("maps/mazes/maze512-8-1.map", 238, 10, 235, 3, 8.24264);
	//runSingleSearch("maps/sc1/Caldera.map", 217, 74, 209, 82, 11.3137);
	//runSingleSearch("maps/sc1/Caldera.map", 215, 463, 196, 453, 23.1421);
	//runSingleSearch("maps/sc1/Caldera.map", 178, 176, 179, 227, 51.4142);
	//runSingleSearch("maps/sc1/Caldera.map", 487, 363, 255, 329, 246.083);
	//runSingleSearch("maps/sc1/Caldera.map", 56, 481, 470, 29, 677.963);
	//runScenario("scenarios/mazes/maze512-8-1.map.scen");

	//decomposer.decompose(&tMap, 0, 128, 1);
	//decomposer.decompose(&tMap, 64, 0, 1);
	//decomposer.decompose(&tMap, 64, 128, 1);
	//gatewayMap.printAll(outputLog);
	//PathfindingStateSpace *stateSpace = new GatewayPathfindingStateSpace(&gatewayMap2);
	//PathfindingStateSpace *stateSpace2 = new PathfindingStateSpace(gatewayMap2.getTraversibleMap());
	//runAStar(stateSpace, stateSpace->getStateID(1,1), stateSpace->getStateID(360,150), 0);
	//runAStar(stateSpace, stateSpace->getStateID(1,1), stateSpace2->getStateID(360,150), 0);
	//runScenario("scenarios/sc1/AcrosstheCape.map.scen", stateSpace);

	//runScenario("scenarios/sc1/AcrosstheCape.map.scen");

	delete gatewayMap;
	delete stateSpaceGateways;

	errorFile->close();
}

void testShitMain()
{
	loggingSearch = false;

	outputLog = 0;
	verbose = false;

	GatewayMap *gatewayMap = new GatewayMap("mapdata/sc1/Backwoods.map.dat");

	GatewayPathfindingStateSpace *stateSpaceGateways = new GatewayPathfindingStateSpace(gatewayMap);
	PathfindingStateSpace *stateSpaceOctile = new PathfindingStateSpace(gatewayMap->getTraversibleMap());
	PathfindingStateSpace *stateSpaceBreadthFirst = new GateDistanceStateSpace(gatewayMap);

	cout << "Search:" << endl;

	int startTime = clock();
	runSingleSearch(stateSpaceGateways, 14, 48, 349, 359, 593.044, "logs/Backwoods_FullSearch_gateway.log");
	int endTime = clock();
	cout << "    Gateways: " << (double)(endTime - startTime) / (double)CLOCKS_PER_SEC << endl;

	startTime = clock();
	runSingleSearch(stateSpaceOctile, 14, 48, 349, 359, 593.044, "logs/Backwoods_FullSearch_octile.log");
	endTime = clock();
	cout << "    Octile: " << (double)(endTime - startTime) / (double)CLOCKS_PER_SEC << endl;

	startTime = clock();
	runSingleSearch(stateSpaceBreadthFirst, 14, 48, 349, 359, 593.044, "logs/Backwoods_FullSearch_BreadthFirst.log");
	endTime = clock();
	cout << "    Breadth first: " << (double)(endTime - startTime) / (double)CLOCKS_PER_SEC << endl;


	int startID = stateSpaceGateways->getStateID(14, 48);
	int goalID = stateSpaceGateways->getStateID(349, 359);

	cout << "Evaluate 800000:" << endl;

	startTime = clock();
	for(int i = 0; i < 800000; ++i)
	{
		stateSpaceGateways->evaluate(startID, goalID);
	}
	endTime = clock();
	cout << "    Gateways: " << (double)(endTime - startTime) / (double)CLOCKS_PER_SEC << endl;

	startTime = clock();
	for(int i = 0; i < 800000; ++i)
	{
		stateSpaceOctile->evaluate(startID, goalID);
	}
	endTime = clock();
	cout << "    Octile: " << (double)(endTime - startTime) / (double)CLOCKS_PER_SEC << endl;

	startTime = clock();
	for(int i = 0; i < 800000; ++i)
	{
		stateSpaceBreadthFirst->evaluate(startID, goalID);
	}
	endTime = clock();
	cout << "    Breadth first: " << (double)(endTime - startTime) / (double)CLOCKS_PER_SEC << endl;

	cout << "Get successors 100000:" << endl;

	startTime = clock();
	for(int i = 0; i < 100000; ++i)
	{
		stateSpaceGateways->getSuccessors(startID);
	}
	endTime = clock();
	cout << "    Gateways: " << (double)(endTime - startTime) / (double)CLOCKS_PER_SEC << endl;

	startTime = clock();
	for(int i = 0; i < 100000; ++i)
	{
		stateSpaceOctile->getSuccessors(startID);
	}
	endTime = clock();
	cout << "    Octile: " << (double)(endTime - startTime) / (double)CLOCKS_PER_SEC << endl;

	startTime = clock();
	for(int i = 0; i < 100000; ++i)
	{
		stateSpaceBreadthFirst->getSuccessors(startID);
	}
	endTime = clock();
	cout << "    Breadth first: " << (double)(endTime - startTime) / (double)CLOCKS_PER_SEC << endl;

}
int main(int argc, char** argv)
{
	//singleSearchMain();

	//GatewayMap *map = new GatewayMap("mapdata/sc1/Aurora.map2.dat");
	//map->printAll(&cout);
	/*cout << "Gatecount: " << map->getGateCount() << endl;
	map->writeMapData("mapdata/sc1/Aurora.map3.dat");
	delete map;
	map = new GatewayMap("mapdata/sc1/Aurora.map3.dat");
	cout << "Gatecount: " << map->getGateCount() << endl;
	delete map;
	map = 0;*/

	//bulkSearchMain(true, true, 0, 1, 0, 50, true);
	//bulkSearchMain(false, true, 1, 2, 0, 50, true);
	//bulkSearchMain(true, false, 2, 3, 0, 50, false);
	//bulkSearchMain(true, true, 3, 4, 0, 50, false);

	//bulkSearchMain(false, true, 0, 69, 0, 500000, false);

	bulkSearchMain(false, true, 2, 3, 0, 500000, false);
	bulkSearchMain(false, true, 4, 5, 0, 500000, false);
	bulkSearchMain(false, true, 11, 12, 0, 500000, false);
	bulkSearchMain(false, true, 14, 15, 0, 500000, false);
	bulkSearchMain(false, true, 15, 16, 0, 500000, false);
	bulkSearchMain(false, true, 22, 23, 0, 500000, false);
	bulkSearchMain(false, true, 24, 25, 0, 500000, false);
	bulkSearchMain(false, true, 27, 28, 0, 500000, false);
	bulkSearchMain(false, true, 29, 30, 0, 500000, false);

	//decomposeMain(69, 86);

	//calculateDistancesMain(29, 30);
	//singleSearchMain();
	//bulkSearchMain(29, 30, 0, 50000000, true);

/*
	bulkSearchMain(14, 15, 2000, 1);
	bulkSearchMain(14, 15, 2001, 1);
	bulkSearchMain(14, 15, 2002, 1);
	bulkSearchMain(14, 15, 2003, 1);
	bulkSearchMain(14, 15, 2004, 1);
	bulkSearchMain(14, 15, 2005, 1);
	bulkSearchMain(14, 15, 2006, 1);
	bulkSearchMain(14, 15, 2007, 1);
	bulkSearchMain(14, 15, 2008, 1);
	bulkSearchMain(14, 15, 2009, 1);
*/
/*
	calculateDistancesMain(2, 3);
	calculateDistancesMain(4, 5);
	calculateDistancesMain(11, 12);
	calculateDistancesMain(14, 15);
	calculateDistancesMain(15, 16);
	calculateDistancesMain(22, 23);
	calculateDistancesMain(24, 25);
	calculateDistancesMain(27, 28);
	calculateDistancesMain(29, 30);

	bulkSearchMain(2, 3, 0, 50000000, true);
	bulkSearchMain(4, 5, 0, 50000000, true);
	bulkSearchMain(11, 12, 0, 50000000, true);
	bulkSearchMain(14, 15, 0, 50000000, true);
	bulkSearchMain(15, 16, 0, 50000000, true);
	bulkSearchMain(22, 23, 0, 50000000, true);
	bulkSearchMain(24, 25, 0, 50000000, true);
	bulkSearchMain(27, 28, 0, 50000000, true);
	bulkSearchMain(29, 30, 0, 50000000, true);
*/
	//decomposeMain(30, 69);
	//calculateDistancesMain(30, 69);

	//bulkSearchMain(32, 69, 0, 50000000, false);
	//bulkSearchMain(0, 1, 0, 50000000, true);

	//decomposeMain(4, 5);
	//calculateDistancesMain(4, 5);
/*
	bulkSearchMain(0, 1, 0, 2940, true);
	bulkSearchMain(1, 2, 0, 1810, true);
	bulkSearchMain(2, 3, 0, 2160, true);
	bulkSearchMain(3, 4, 0, 4100, true);
	//bulkSearchMain(4, 5, 0, 2990);
	bulkSearchMain(5, 6, 0, 2430, true);
	bulkSearchMain(6, 7, 0, 1790, true);
	bulkSearchMain(7, 8, 0, 3760, true);
	bulkSearchMain(8, 9, 0, 3060, true);
	bulkSearchMain(9, 10, 0, 2780, true);
	bulkSearchMain(10, 11, 0, 2150, true);
	bulkSearchMain(11, 12, 0, 1690, true);
	bulkSearchMain(12, 13, 0, 2640, true);
	bulkSearchMain(13, 14, 0, 4000, true);


	bulkSearchMain(14, 15, 0, 2810);
	bulkSearchMain(14, 15, 0, 2810, true);
	bulkSearchMain(15, 16, 0, 1840);
	bulkSearchMain(15, 16, 0, 1840, true);
	bulkSearchMain(16, 17, 0, 2770);
	bulkSearchMain(16, 17, 0, 2770, true);
	bulkSearchMain(17, 18, 0, 2530);
	bulkSearchMain(17, 18, 0, 2530, true);
	bulkSearchMain(18, 19, 0, 1860);
	bulkSearchMain(18, 19, 0, 1860, true);
	bulkSearchMain(19, 20, 0, 1980);
	bulkSearchMain(19, 20, 0, 1980, true);
	bulkSearchMain(20, 21, 0, 2430);
	bulkSearchMain(20, 21, 0, 2430, true);
	bulkSearchMain(21, 22, 0, 3270);
	bulkSearchMain(21, 22, 0, 3270, true);
	bulkSearchMain(22, 23, 0, 1670);
	bulkSearchMain(22, 23, 0, 1670, true);
	bulkSearchMain(23, 24, 0, 1790);
	bulkSearchMain(23, 24, 0, 1790, true);
	bulkSearchMain(24, 25, 0, 3190);
	bulkSearchMain(24, 25, 0, 3190, true);
	bulkSearchMain(25, 26, 0, 2520);
	bulkSearchMain(25, 26, 0, 2520, true);
	bulkSearchMain(26, 27, 0, 2740);
	bulkSearchMain(26, 27, 0, 2740, true);
	bulkSearchMain(27, 28, 0, 2300);
	bulkSearchMain(27, 28, 0, 2300, true);
	bulkSearchMain(28, 29, 0, 2600);
	bulkSearchMain(28, 29, 0, 2600, true);
	bulkSearchMain(29, 30, 0, 2590);
	bulkSearchMain(29, 30, 0, 2590, true);
*/

	//decomposeSingleBMPOnly("AcrosstheCape", "sc1");
	//decomposeSingleBMPOnly("Backwoods", "sc1");
	//decomposeSingleBMPOnly("BigGameHunters", "sc1");
	//decomposeSingleBMPOnly("FloodedPlains", "sc1");
	//decomposeSingleBMPOnly("Cauldron", "sc1");
	//decomposeSingleBMPOnly("AR0205SR", "bg2");
	//decomposeMain(30, 32);
	//bulkSearchMain(30, 32);
/*
	for(int i = 11; i < 12; ++i)
	{
		GatewayMap *gatewayMap;
		string mapName, gameName;

		switch(i) {
		case 0: mapName = "AcrosstheCape"; gameName = "sc1"; break;
		case 1: mapName = "Aftershock"; gameName = "sc1"; break;
		case 2: mapName = "Archipelago"; gameName = "sc1"; break;
		case 3: mapName = "ArcticStation"; gameName = "sc1"; break;
		case 4: mapName = "Aurora"; gameName = "sc1"; break;
		case 5: mapName = "Backwoods"; gameName = "sc1"; break;
		case 6: mapName = "BigGameHunters"; gameName = "sc1"; break;
		case 7: mapName = "BlackLotus"; gameName = "sc1"; break;
		case 8: mapName = "BlastFurnace"; gameName = "sc1"; break;
		case 9: mapName = "BrokenSteppes"; gameName = "sc1"; break;
		case 10: mapName = "Brushfire"; gameName = "sc1"; break;
		case 11: mapName = "Caldera"; gameName = "sc1"; break;
		case 12: mapName = "CatwalkAlley"; gameName = "sc1"; break;
		case 13: mapName = "Cauldron"; gameName = "sc1"; break;
		case 14: mapName = "CrashSites"; gameName = "sc1"; break;
		case 15: mapName = "CrescentMoon"; gameName = "sc1"; break;
		case 16: mapName = "Crossroads"; gameName = "sc1"; break;
		case 17: mapName = "DarkContinent"; gameName = "sc1"; break;
		case 18: mapName = "Desolation"; gameName = "sc1"; break;
		case 19: mapName = "EbonLakes"; gameName = "sc1"; break;
		case 20: mapName = "Elderlands"; gameName = "sc1"; break;
		case 21: mapName = "Enigma"; gameName = "sc1"; break;
		case 22: mapName = "Entanglement"; gameName = "sc1"; break;
		case 23: mapName = "Eruption"; gameName = "sc1"; break;
		case 24: mapName = "Expedition"; gameName = "sc1"; break;
		case 25: mapName = "FireWalker"; gameName = "sc1"; break;
		case 26: mapName = "FloodedPlains"; gameName = "sc1"; break;
		case 27: mapName = "GhostTown"; gameName = "sc1"; break;
		case 28: mapName = "GladiatorPits"; gameName = "sc1"; break;
		case 29: mapName = "GreenerPastures"; gameName = "sc1"; break;
		case 30: mapName = "AR0205SR"; gameName = "bg2"; break;
		case 31: mapName = "AR0406SR"; gameName = "bg2"; break;
		case 32: mapName = "byzantium"; gameName = "sc1"; break;
		}

		decomposeSingleBMPOnly(mapName, gameName);
	}
*/
	//decomposeMain(0, 30);
	//calculateDistancesMain(0, 1);
	//bulkSearchMain(0, 1);

	//decomposeMain(0, 5);
	//decomposeMain(5, 9);
	//decomposeMain(9, 12);
	//decomposeMain(12, 15);
	//decomposeMain(15, 30);

	//calculateDistancesMain(0, 5);
	//calculateDistancesMain(5, 9);
	//calculateDistancesMain(9, 12);
	//calculateDistancesMain(12, 15);
	//calculateDistancesMain(15, 30);



/*
//	bulkSearchMain(0, 1);
	//bulkSearchMain(1, 2);
//	bulkSearchMain(2, 7);
	bulkSearchMain(4, 7);
	//bulkSearchMain(6, 7);
	bulkSearchMain(7, 11);
	//bulkSearchMain(11, 12);
	bulkSearchMain(12, 15);
	//bulkSearchMain(15, 16);
	bulkSearchMain(16, 18);
	//bulkSearchMain(18, 20);
	bulkSearchMain(20, 22);
	//bulkSearchMain(22, 24);
	bulkSearchMain(24, 30);
*/
	//TODO:
	//REMEMBER TO!!
	//Check specifically GreenPastures on start: (753,123) & goal: (93,438)


	//singleSearchMain();
	//testShitMain();
	return 0;
}
