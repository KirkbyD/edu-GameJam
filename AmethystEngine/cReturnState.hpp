#pragma once
#include "cFSMState.hpp"
#include "MapInfo.hpp"
#include "nAI.hpp"

class ReturnState : public cFSMState {
	bool hasBasePath = false;
	nAI::cAIAlgorithms aiAlgorithms;
	std::list<glm::vec3> posList;

public:
	ReturnState(void);
	virtual ~ReturnState(void);

	virtual void Update(void);

	virtual void OnEnterState(void);
	virtual void OnExitState(void);

};
