/*
 * StateSpace.h
 *
 *  Created on: Dec 2, 2014
 *      Author: kaha
 */

#ifndef STATESPACE_H_
#define STATESPACE_H_

#include <ostream>
#include <vector>

using namespace std;

namespace AStar {

class StateSpace {
public:
	//StateSpace();
	virtual ~StateSpace() {};

	virtual vector<pair<int, double> > getSuccessors(int stateID) = 0;
	virtual double evaluate(int stateID, int goalState) = 0;
	virtual bool isPossibleState(int stateID) = 0;
	virtual void writeStateToStream(int stateID, ostream *out) = 0;
	virtual bool checkPossibleGoal(int stateID, double pathLength) { return false; };
};

}

#endif /* STATESPACE_H_ */
