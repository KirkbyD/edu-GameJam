#pragma once
#include "iCommand.hpp"
#include "cCommandManager.hpp"

class cRotate : public iCommand {
	bool _UpdateHasBeenCalled;
	bool _isRelative;
	size_t _subsystem;
	std::string _target;
	glm::quat _start_orientation;
	glm::quat _current_orientation;
	glm::quat _end_orientation;
	glm::quat _rotational_speed;
	float _time, _currentTime;
	double _time_elapsed;
	float _easeIn, _easeOut;
	bool _isTime;
	float _easein_amount;

	float Clamp(float x);
	virtual void AddSerialCommand(iCommand* pCommand);
	virtual void AddParallelCommand(iCommand* pCommand);

public:
	// move time
	cRotate() {
		_UpdateHasBeenCalled = false;
		_isRelative = true;
	}

	virtual void Init(sData data);
	virtual bool Update(double deltatime);
	virtual float SmootherStep(float startdist, float enddist, float currdist);
};
