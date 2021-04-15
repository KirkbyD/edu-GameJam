#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#include "cMove.hpp"

void cMove::Init(sData data) {
	this->_subsystem = data.getSubsystem();
	this->_target = data.getTarget();
	this->_end_position = glm::vec3(data.getVec4Data()[1]);
	if (data.getVec4Data()[0] != glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)) { _isRelative = false; }
	if (!_isRelative) { _start_position = glm::vec3(data.getVec4Data()[0]); }
	this->_time = data.getFloatData()[0];
	if (data.isEase()) { this->_easeIn = data.getFloatData()[1]; this->_easeOut = data.getFloatData()[2]; }
	if (data.isEaseTime() == true) { this->_isTime = true; }
	else { this->_isTime = false; }
	this->_easeIn_step = 0.0f;
	this->_easeOut_step = 0.0f;
	this->_currentTime = 0.0f;
}

bool cMove::Update(double deltatime) {
	cCommandManager* pcmdmngr = cCommandManager::GetCommandManager();

	if (_subsystem == CAMERA) {
		sData data;
		data.setSubsystem(_subsystem);
		data.setCMD(GET_POSITION);
		data.setTarget(_target);
		pcmdmngr->IssueCommand(data);

		_current_pos = glm::vec3(data.getVec4Data()[0].x, data.getVec4Data()[0].y, data.getVec4Data()[0].z);

		if (!this->_UpdateHasBeenCalled) {
			if (this->_isRelative)
				this->_start_position = _current_pos;

			// Calculate the velocity...
			glm::vec3 moveVector = this->_end_position - this->_start_position;
			this->_total_distance = glm::length(moveVector);
			float speed = this->_total_distance / this->_time;
			glm::vec3 direction = glm::normalize(moveVector);
			this->_velocity = direction * speed;

			// need to set the position of the model selected to the starting position...
			this->_UpdateHasBeenCalled = true;
		}

		float distance = glm::distance(this->_current_pos, this->_start_position);

		if (distance >= this->_total_distance)
			return true;

		glm::vec3 deltaStep;

		if ((this->_easeIn != 0.0f || this->_easeOut != 0.0f)) {
			// Easeing ( Distance )
			if (this->_isTime == false) {
				if (distance != 0) {
					float easein_percent = (this->_easeIn/100) * this->_total_distance;
					float easeout_percent = this->_total_distance - ((this->_easeOut/100) * this->_total_distance);

					if (this->_easeIn != 0.0f && distance < easein_percent)
						deltaStep = this->_velocity * (float)deltatime * SmootherStep(((distance - easein_percent) / 4), easein_percent, distance);
					else if (this->_easeOut != 0.0f && distance > easeout_percent)
						deltaStep = this->_velocity * (float)deltatime * (SmootherStep(this->_total_distance, easeout_percent, distance) < 0.1f ? 0.1f : SmootherStep(this->_total_distance, easeout_percent, distance));
					else
						deltaStep = this->_velocity * (float)deltatime;
				}
				else
					deltaStep = this->_velocity * (float)deltatime;
			}
			// Easeing ( Time )
			else {
				this->_currentTime += deltatime;
				if (this->_currentTime != 0) {
					if (this->_easeIn != 0.0f && _currentTime < _easeIn)
						deltaStep = this->_velocity * (float)deltatime * SmootherStep(0, _easeIn, _currentTime);
					else if (this->_easeOut != 0.0f && _currentTime > (_time - _easeOut))
						deltaStep = this->_velocity * (float)deltatime * (SmootherStep(_time, (_time - _easeOut), _currentTime) < 0.1f ? 0.1f : SmootherStep(_time, (_time - _easeOut), _currentTime));
					else
						deltaStep = this->_velocity * (float)deltatime;
				}
				else
					deltaStep = this->_velocity * (float)deltatime;
			}
		}
		else
			deltaStep = this->_velocity * (float)deltatime;

		// This could be done in the physics engine...
		this->_current_pos += deltaStep;

		// call the physics engine to do the work; check to see if the distance is greater or equal to the total distance

		// set the position on the model
		data.setCMD(SET_POSITION);
		data.setVec4Data(_current_pos);
		pcmdmngr->IssueCommand(data);

		return false;
	}

	else {
		sData data;
		data.setSubsystem(_subsystem);
		data.setCMD(GET_POSITION);
		data.setTarget(_target);
		pcmdmngr->IssueCommand(data);

		_current_pos = glm::vec3(data.getVec4Data()[0].x, data.getVec4Data()[0].y, data.getVec4Data()[0].z);

		// check to see if this is the first time this command was called for this object
		if (!this->_UpdateHasBeenCalled) {
			if (this->_isRelative)
				this->_start_position = _current_pos;

			// need to set the position of the model selected to the starting position...

			// Calculate the velocity...
			glm::vec3 moveVector = this->_end_position - this->_start_position;
			this->_total_distance = glm::length(moveVector);
			float speed = this->_total_distance / this->_time;
			glm::vec3 direction = glm::normalize(moveVector);
			this->_velocity = direction * speed;

			// set the velocity on the model
			sData data;
			data.setSubsystem(MODELS);
			data.setCMD(SET_VELOCITY);
			data.setTarget(_target);
			data.setVec4Data(_velocity);
			pcmdmngr->IssueCommand(data);

			this->_UpdateHasBeenCalled = true;
		}

		float distance = glm::distance(this->_current_pos, this->_start_position);

		if (distance >= this->_total_distance)
			return true;

		glm::vec3 deltaStep;

		if ((this->_easeIn != 0.0f || this->_easeOut != 0.0f)) {
			// Easeing ( Distance )
			if (this->_isTime == false) {
				if (distance != 0) {
					float easein_percent = this->_easeIn * this->_total_distance;
					float easeout_percent = this->_total_distance - (this->_easeOut * this->_total_distance);

					if (this->_easeIn != 0.0f && distance < easein_percent)
						deltaStep = this->_velocity * (float)deltatime * SmootherStep(((distance - easein_percent) / 4), easein_percent, distance);
					else if (this->_easeOut != 0.0f && distance > easeout_percent)
						deltaStep = this->_velocity * (float)deltatime * (SmootherStep(this->_total_distance, easeout_percent, distance) < 0.1f ? 0.1f : SmootherStep(this->_total_distance, easeout_percent, distance));
					else
						deltaStep = this->_velocity * (float)deltatime;
				}
				else
					deltaStep = this->_velocity * (float)deltatime;
			}
			// Easeing ( Time )
			else {
				this->_currentTime += deltatime;
				if (this->_currentTime != 0) {
					if (this->_easeIn != 0.0f && _currentTime < _easeIn)
						deltaStep = this->_velocity * (float)deltatime * SmootherStep(0, _easeIn, _currentTime);
					else if (this->_easeOut != 0.0f && _currentTime > (_time - _easeOut))
						deltaStep = this->_velocity * (float)deltatime * (SmootherStep(_time, (_time - _easeOut), _currentTime) < 0.1f ? 0.1f : SmootherStep(_time, (_time - _easeOut), _currentTime));
					else
						deltaStep = this->_velocity * (float)deltatime;
				}
				else
					deltaStep = this->_velocity * (float)deltatime;
			}
		}
		else
			deltaStep = this->_velocity * (float)deltatime;

		// This could be done in the physics engine...
		this->_current_pos += deltaStep;

		// call the physics engine to do the work; check to see if the distance is greater or equal to the total distance

		// set the position on the model
		data.setCMD(SET_POSITION);
		data.setVec4Data(_current_pos);
		pcmdmngr->IssueCommand(data);

		return false;
	}
}


float cMove::SmootherStep(float startdist, float enddist, float currdist) {
	// Scale, and clamp x to 0..1 range
	currdist = Clamp((currdist - startdist) / (enddist - startdist));

	// Evaluate polynomial
	return currdist * currdist * currdist * (currdist * (currdist * 6 - 15) + 10);
}

float cMove::Clamp(float x) {
	if (x < 0.0f) { return 0.0f; }
	if (x > 1.0f) { return 1.0f; }
	return x;
}

void cMove::AddSerialCommand(iCommand* pCommand) { }
void cMove::AddParallelCommand(iCommand* pCommand) { }
