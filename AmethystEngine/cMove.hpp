#pragma once
#include "cCommandManager.hpp"
#include "glm/glm.hpp"

class cMove : public iCommand {
	bool _UpdateHasBeenCalled;
	bool _isRelative, _isTime;
	size_t _subsystem;
	std::string _target;
	glm::vec3 _start_position;
	glm::vec3 _end_position;
	glm::vec3 _current_pos;
	glm::vec3 _velocity;
	float _time;
	double _currentTime;
	float _total_distance;
	float _easeIn, _easeOut;
	float _easeIn_step, _easeOut_step;

	virtual float Clamp(float x);
	virtual void AddSerialCommand(iCommand* pCommand);
	virtual void AddParallelCommand(iCommand* pCommand);

public:
	// move time
	cMove() {
		_UpdateHasBeenCalled = false;
		_isRelative = true;
	}

	virtual void Init(sData data);
	virtual bool Update(double deltatime);
	virtual float SmootherStep(float startdist, float enddist, float currdist);
};
