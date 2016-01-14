/*
 * AStarNode.h
 *
 *  Created on: Dec 1, 2014
 *      Author: kaha
 */

#ifndef ASTARNODE_H_
#define ASTARNODE_H_

namespace AStar {

class AStarNode {
private:
	int parentState;
	int stateID;
	double f;
	double g;

public:
	AStarNode(int parentState, int stateID, double g, double h);

	double getF() const;
	double getG() const;
	//double getH() const;
	int getParentState() const;
	int getStateID() const;

	friend bool operator <(const AStarNode &node1, const AStarNode &node2);
};

}

#endif /* ASTARNODE_H_ */
