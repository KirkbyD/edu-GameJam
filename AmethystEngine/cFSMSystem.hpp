#pragma once
#include <vector>
#include <string>

#include "cFSMState.hpp"

class cFSMSystem {
public:
	cFSMSystem(void);
	virtual ~cFSMSystem(void);

	void AddState(cFSMState* state);
	void TransitionToState(cFSMState* state);

	void Update(void);

	void Start(void);
	void Reset(void);

	void Init();

private:
	std::vector<cFSMState*> mStateVec; //keeps track of all the states our FSM can enter
	bool mIsRunning; //is our FSM currently active

	cFSMState* mCurrentState;
};
