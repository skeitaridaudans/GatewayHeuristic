/*
 * AStar.h
 *
 *  Created on: Dec 1, 2014
 *      Author: kaha
 */

#ifndef ASTAR_H_
#define ASTAR_H_

#include <tr1/unordered_map>
//#include <set>
#include <queue>
#include <deque>
#include "StateSpace.h"
#include "AStarNode.h"

using namespace std;
using namespace std::tr1;

namespace AStar {

class AStar {
private:
	StateSpace *stateSpace;
	int goalStateID;

	unordered_map<int, AStarNode> closedList;
	//MULTI_SET
	//multiset<AStarNode> openList;
	//PRIORITY_QUEUE
	priority_queue<AStarNode> openList;
public:
	AStar(StateSpace *stateSpace, int startStateID, int goalStateID);
	AStar(StateSpace *stateSpace, vector<int> startStateIDs);
	virtual ~AStar();

	pair<deque<int>, double> findSolution(bool logging, ostream *out, int &out_expanded);
	void findMultipleSolutions();

};

}

#endif /* ASTAR_H_ */
