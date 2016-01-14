/*
 * AStarNode.cpp
 *
 *  Created on: Dec 1, 2014
 *      Author: kaha
 */

#include "AStarNode.h"

namespace AStar {

AStarNode::AStarNode(int parentState, int stateID, double g, double h)
{
	this->parentState = parentState;
	this->stateID = stateID;
	this->g = g;
	this->f = g + h;
}

double AStarNode::getF() const
{
	return f;
}

double AStarNode::getG() const
{
	return g;
}
/*
double AStarNode::getH() const
{
	return f - g;
}
*/
int AStarNode::getParentState() const
{
	return parentState;
}

int AStarNode::getStateID() const
{
	return stateID;
}

bool operator <(const AStarNode &node1, const AStarNode &node2)
{
	//PRIORITY_QUEUE
	if(node1.getF() == node2.getF()){
		return (node1.getG() < node2.getG()); }
	else{
		return (node1.getF() > node2.getF()); }

	//MULTI_SET
	//if(node1.getF() == node2.getF()){
	//	return (node1.getG() > node2.getG()); }
	//else{
	//	return (node1.getF() < node2.getF()); }
}

}
