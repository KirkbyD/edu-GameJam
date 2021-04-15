#pragma once
#include "iCommand.hpp"
#include "cCommandManager.hpp"

class cCommandGroup : public iCommand {
public:
	virtual void Init(sData data);
	virtual bool Update(double deltatime);
	virtual float SmootherStep(float startdist, float enddist, float currdist);

	virtual void AddSerialCommand(iCommand* cmd);
	virtual void AddParallelCommand(iCommand* cmd);



private:
	virtual float Clamp(float x);

	std::vector<iCommand*> _parallel_list;
	std::queue<iCommand*> _serial_list;
};
