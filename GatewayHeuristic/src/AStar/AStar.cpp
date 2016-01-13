/*
 * AStar.cpp
 *
 *  Created on: Dec 1, 2014
 *      Author: kaha
 */

#include <iostream>
#include <tr1/unordered_map>
#include "AStar.h"

using namespace std;
using namespace std::tr1;

namespace AStar {

AStar::AStar(StateSpace *stateSpace, int startStateID, int goalStateID)
{
	this->stateSpace = stateSpace;
	this->goalStateID = goalStateID;

	AStarNode startNode(-1, startStateID, 0, stateSpace->evaluate(startStateID, goalStateID));
	openList.push(startNode);
}

AStar::AStar(StateSpace *stateSpace, vector<int> startStateIDs)
{
	this->stateSpace = stateSpace;
	this->goalStateID = 0;

	for(vector<int>::iterator p_stateID = startStateIDs.begin(); p_stateID != startStateIDs.end(); ++p_stateID)
	{
		openList.push(AStarNode(-1, *p_stateID, 0, 0));
	}
}

AStar::~AStar()
{
	// TODO Auto-generated destructor stub
}

pair<deque<int>, double> AStar::findSolution(bool logging, ostream *out, int &out_expanded, int &out_reexpanded)
{
	out_expanded = 0;
	out_reexpanded = 0;
	while(!openList.empty())
	{
		AStarNode node(0,0,0,0);
		bool isInClosedList;
		do {
			node = openList.top();
			openList.pop();
			unordered_map<int, AStarNode>::iterator closedNode
								= closedList.find(node.getStateID());
			if(closedNode == closedList.end()) {
				isInClosedList = false;
			}
			else {
				//if(node.getG() < closedNode->second.getG()) {  //too many re-expansions, floating point error
				if(closedNode->second.getG() - node.getG() > 0.1) {
					closedList.erase(closedNode);
					isInClosedList = false;
					++out_reexpanded;
/*					*out << "Node ";
					stateSpace->writeStateToStream(node.getStateID(), out);
					*out << " with G=" << node.getG();
					*out << " found on closed list with G=" << closedNode->second.getG();
					*out << " - Difference=" << closedNode->second.getG()-node.getG() << endl;*/
				}
				else {
					isInClosedList = true;
				}
			}
		} while(isInClosedList && !openList.empty());

		if(isInClosedList)
		{
			return pair<deque<int>, double>(deque<int>(), 0.0);
		}

		if(logging) {
			*out << "closed ";
			stateSpace->writeStateToStream(node.getStateID(), out);
		}

		++out_expanded;

		closedList.insert(pair<int,AStarNode>(node.getStateID(), node));
		if(node.getStateID() == goalStateID)
		{
			if(logging) {
				*out << endl;
			}
			break;
		}
		else
		{
			if(logging) {
				*out << " opened ";
			}
			vector<pair<int, double> > moves = stateSpace->getSuccessors(node.getStateID());
			for (vector<pair<int, double> >::iterator move = moves.begin(); move != moves.end(); ++move)
			{
				//if(closedList.find((*move).first) == closedList.end())
				{
					openList.push(AStarNode(node.getStateID(), (*move).first, node.getG() + (*move).second, stateSpace->evaluate((*move).first, goalStateID)));
					if(logging) {
						*out << " "; stateSpace->writeStateToStream((*move).first, out);
					}
				}
			}
			if(logging) {
				*out << endl;
			}
		}
	}
/*
	cout << "Size of closed list: " << closedList.size() << endl;
	cout << "Size of open list:   " << openList.size() << endl;
*/
	deque<int> solution;

	AStarNode solutionNode = closedList.find(goalStateID)->second;
	solution.push_front(solutionNode.getStateID());
	double solutionCost = solutionNode.getG();
	while(solutionNode.getParentState() != -1)
	{
		solutionNode = closedList.find(solutionNode.getParentState())->second;
		solution.push_front(solutionNode.getStateID());
/*
		double heuristic = stateSpace->evaluate(solutionNode.getStateID(), goalStateID);
		double cost = solutionCost - solutionNode.getG();
		if(heuristic - cost > 0.0001)
		{
			cout << "Heuristic error from node: " << solutionNode.getStateID() << " to goal: " << goalStateID
					<< " - Heuristic: " << heuristic
					<< " - Cost: " << cost
					<< " - Difference: " << heuristic - cost
					<< endl;
		}*/
	}

	return pair<deque<int>, double>(solution, solutionCost);
}


void AStar::findMultipleSolutions()
{
	while(!openList.empty())
	{
		AStarNode node(0,0,0,0);
		bool isInClosedList;
		do {
			node = openList.top();
			openList.pop();
			isInClosedList = (closedList.find(node.getStateID()) != closedList.end());
		} while(isInClosedList && !openList.empty());

		if(isInClosedList)
		{
			return;
		}

		closedList.insert(pair<int,AStarNode>(node.getStateID(), node));

		if(stateSpace->checkPossibleGoal(node.getStateID(), node.getG())) {
			return;
		}

		vector<pair<int, double> > moves = stateSpace->getSuccessors(node.getStateID());
		for (vector<pair<int, double> >::iterator move = moves.begin(); move != moves.end(); ++move)
		{
			if(closedList.find((*move).first) == closedList.end())
			{
				openList.push(AStarNode(node.getStateID(), (*move).first, node.getG() + (*move).second, stateSpace->evaluate((*move).first, goalStateID)));
			}
		}
	}
}

}
