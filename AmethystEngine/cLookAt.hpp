#pragma once
#include "iCommand.hpp"
#include "cCommandManager.hpp"

class cLookAt : public iCommand {
	size_t _subsystem;
	std::string _target;

	virtual float Clamp(float x);
	virtual void AddSerialCommand(iCommand* pCommand);
	virtual void AddParallelCommand(iCommand* pCommand);
	virtual float SmootherStep(float startdist, float enddist, float currdist);

public:
	cLookAt() {
		_subsystem = 0;
		_target = "";
	};
	
	virtual void Init(sData data);
	virtual bool Update(double deltatime);
};
