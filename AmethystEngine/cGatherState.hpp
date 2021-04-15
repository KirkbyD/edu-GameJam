#pragma once
#include "cFSMState.hpp"
#include "MapInfo.hpp"

class GatherState : public cFSMState {
public:
	GatherState(void);
	virtual ~GatherState(void);

	virtual void Update(void);

	virtual void OnEnterState(void);
	virtual void OnExitState(void);
};
