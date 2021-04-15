#pragma once
#include <glm/glm.hpp>
#include "iCommand.hpp"

class cFollow : public iCommand {
private:
	bool _UpdateHasBeenCalled;
	bool _isRelative;
	bool _isObject;
	float _curve_percent;			// in the form of a decimal ( 0.01 = 1% )
	size_t _subsystem;
	std::string _target, _source;
	glm::vec3 _start_position;
	glm::vec3 _current_pos;
	glm::vec3 _end_position;
	glm::vec3 _control_point;
	glm::vec3 _target_offset;
	float _max_percent, _max_speed;
	float _easeIn, _easeOut;
	float _easeInDist, _easeOutDist;
	float _inner_radius, _outer_radius;
	float _time, _current_time;

	virtual float Clamp(float x);
	virtual void AddSerialCommand(iCommand* pCommand);
	virtual void AddParallelCommand(iCommand* pCommand);

public:
	cFollow() {
		_UpdateHasBeenCalled = false;
		_isRelative = true;
		_isObject = false;
	}

	virtual void Init(sData data);
	virtual bool Update(double deltatime);
	virtual float SmootherStep(float startdist, float enddist, float currdist);
};
