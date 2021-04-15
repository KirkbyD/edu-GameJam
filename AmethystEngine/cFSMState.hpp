#pragma once
#include <string>
#include <map>

class cFSMState {
public:
	cFSMState(std::string name);
	virtual ~cFSMState(void);

	//Action to take when current state is active
	virtual void Update(void) = 0;
	//Action to take when current state is being entered
	virtual void OnEnterState(void) = 0;
	//Action to take when current state is being exited from
	virtual void OnExitState(void) = 0;

	bool IsDone(void); //a condition has been met that triggers a transition

	void AddTransition(int condition, cFSMState* state); //add a new transition to mTransitionMap, includes condition as map key
	cFSMState* GetTransition(); //returns state that we want to transition to
protected:
	cFSMState(void);
	int mCurrentCondition; //0 = state has not performed action, 1 = state has perfomred action, is now ready to switch states

private:
	std::string mStateName;
	std::map<int, cFSMState*> mTransitionMap; //holds all the states our current state can transition to
};
