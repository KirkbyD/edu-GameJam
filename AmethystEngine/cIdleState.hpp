#pragma once
#include "cFSMState.hpp"
#include "MapInfo.hpp"

class IdleState : public cFSMState {
public:
	IdleState(void);
	virtual ~IdleState(void);

	virtual void Update(void);

	virtual void OnEnterState(void);
	virtual void OnExitState(void);

};
