/*
 * TraversibleMap.h
 *
 *  Created on: Dec 16, 2014
 *      Author: kaha
 */

#ifndef TRAVERSIBLEMAP_H_
#define TRAVERSIBLEMAP_H_

#include <string>

using namespace std;

namespace Pathfinding {

class TraversibleMap {
private:
	bool **map;
	int width;
	int height;

	string mapFile;

	bool readMap(string mapFile);

public:
	TraversibleMap() {map = 0; width = 0; height = 0;};
	TraversibleMap(bool **map, int width, int height) {this->map = map; this->width = width; this->height = height;};
	TraversibleMap(string mapFile);
	virtual ~TraversibleMap();

	bool **getMap();
	int getWidth();
	int getHeight();
	string getFileName();
};

}

#endif /* TRAVERSIBLEMAP_H_ */
