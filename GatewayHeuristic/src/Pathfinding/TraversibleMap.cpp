/*
 * TraversibleMap.cpp
 *
 *  Created on: Dec 16, 2014
 *      Author: kaha
 */

#include <fstream>
#include "TraversibleMap.h"

namespace Pathfinding {

TraversibleMap::TraversibleMap(string mapFile)
{
	this->mapFile = mapFile;
	if(!readMap(mapFile))
	{
		width = 0;
		height = 0;
		map = 0;
		return;
	}
}

TraversibleMap::~TraversibleMap()
{
	for(int i = 0; i < width; i++)
	{
		delete[] map[i];
	}
	delete[] map;
}

bool TraversibleMap::readMap(string mapFile)
{
	ifstream fin;
	fin.open(mapFile.c_str());
	if(!fin.good())
	{
		return false;
	}
	string text;
	for(int i = 0; i < 2; i++)
	{
		do {
			fin >> text;
		} while(text != "height" && text != "width" && !fin.eof());
		if(fin.eof())
			return false;
		else if(text == "height")
			fin >> height;
		else
			fin >> width;
	}
	do {
		fin >> text;
	} while(text != "map");

	map = new bool*[width];
	for(int i = 0; i < width; i++)
	{
		map[i] = new bool[height];
	}

	char *mapLine = new char[width+1];

	for(int j = 0; j < height; j++)
	{
		fin >> mapLine;
		for(int i = 0; i < width; i++)
		{
			if(mapLine[i] == '.')
				map[i][j] = true;
			else
				map[i][j] = false;
		}
	}

	return true;
}

bool **TraversibleMap::getMap()
{
	return map;
}

int TraversibleMap::getWidth()
{
	return width;
}

int TraversibleMap::getHeight()
{
	return height;
}

string TraversibleMap::getFileName()
{
	return mapFile;
}

}
