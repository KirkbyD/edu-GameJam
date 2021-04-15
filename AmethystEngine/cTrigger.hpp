#pragma once
#include "iCommand.hpp"
#include "cCommandManager.hpp"

class cTrigger : public iCommand {
	virtual float Clamp(float x);
	virtual void AddSerialCommand(iCommand* pCommand);
	virtual void AddParallelCommand(iCommand* pCommand);

	bool _triggered;
	glm::vec3 _position;
	float _radius;
	std::vector<std::string> _vec_trigTargets;

public:
	virtual void Init(sData data);
	virtual bool Update(double deltatime);
	virtual float SmootherStep(float startdist, float enddist, float currdist);

	// Think about the implementation of the command groups and if they are to be a part of the commands or not...
	// virtual void AddCommandSerial(iCommand* pCommand) = 0;
	// virtual void AddCommandParallel( iCommand* pCommand ) = 0;
};
