#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include "sData.hpp"

class iCommand {
public:
	virtual ~iCommand() { };

	virtual void Init(sData data) = 0;
	virtual bool Update( double deltatime ) = 0;			// IsDone & Update now in the same function call, returns if it is done processing the update or not

	// Think about the implementation of the command groups and if they are to be a part of the commands or not...
	virtual void AddSerialCommand(iCommand* pCommand) = 0;
	virtual void AddParallelCommand( iCommand* pCommand ) = 0;
	virtual float SmootherStep(float startdist, float enddist, float currdist) = 0;

private:
	virtual float Clamp(float x) = 0;

};
