#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#include "cRotate.hpp"

void cRotate::Init(sData data) {
	this->_subsystem = data.getSubsystem();
	this->_target = data.getTarget();

	glm::vec3 EulerAngleEnd;
	EulerAngleEnd.x = glm::radians(data.getVec4Data()[1].x);
	EulerAngleEnd.y = glm::radians(data.getVec4Data()[1].y);
	EulerAngleEnd.z = glm::radians(data.getVec4Data()[1].z);
	this->_end_orientation = glm::quat(EulerAngleEnd);

	if (data.getVec4Data()[0] != glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)) { _isRelative = false; }
	if (!_isRelative) {
		glm::vec3 EulerAngle;
		EulerAngle.x = glm::radians(data.getVec4Data()[0].x);
		EulerAngle.y = glm::radians(data.getVec4Data()[0].y);
		EulerAngle.z = glm::radians(data.getVec4Data()[0].z);
		_start_orientation = glm::quat(EulerAngle);
	}

	if (data.isEase()) { this->_easeIn = data.getFloatData()[1]; this->_easeOut = data.getFloatData()[2]; }
	if (data.isEaseTime() == true) { this->_isTime = true; this->_time = data.getFloatData()[0] + _easeIn + _easeOut; }
	else { this->_isTime = false; this->_time = data.getFloatData()[0]; }
	this->_time_elapsed = 0.0;
	this->_easein_amount = 0.0f;
}

bool cRotate::Update(double deltatime) {
	cCommandManager* pcmdmngr = cCommandManager::GetCommandManager();

	if (!this->_UpdateHasBeenCalled) {
		if (this->_isRelative) {
			sData data;
			data.setSubsystem(_subsystem);
			data.setCMD(GET_ORIENTATION);
			data.setTarget(_target);
			pcmdmngr->IssueCommand(data);
			this->_start_orientation = glm::quat(data.getQuatData()[0]);
		}

		this->_UpdateHasBeenCalled = true;
	}

	if (this->_time_elapsed >= this->_time)
		return true;

	float AmountOfRotationComplete = 0.0f;

	this->_time_elapsed += deltatime;

	if ((this->_easeIn != 0.0f || this->_easeOut != 0.0f)) {
		// Easeing ( Time )
		if (this->_isTime == true) {
			if (this->_easeIn != 0.0f && (float)_time_elapsed < _easeIn) {
				AmountOfRotationComplete = (SmootherStep(0, _easeIn, (float)_time_elapsed) / this->_time);
				_easein_amount = AmountOfRotationComplete;
			}
			else if (this->_easeOut != 0.0f && (float)_time_elapsed > (_time - _easeOut)) {
				AmountOfRotationComplete = (SmootherStep(_time, (_time - _easeOut), (float)_time_elapsed) / this->_time) + _easein_amount;
				_easein_amount = AmountOfRotationComplete;
				if (AmountOfRotationComplete >= 1.0f) { AmountOfRotationComplete = 1.0f; }
			}
			else {
				AmountOfRotationComplete = ((float)this->_time_elapsed / this->_time);
				_easein_amount = AmountOfRotationComplete;
			}
		}
		else
			AmountOfRotationComplete = (float)this->_time_elapsed / this->_time;
	}
	else
		AmountOfRotationComplete = (float)this->_time_elapsed / this->_time;

	glm::quat qCurrentOrientation = glm::slerp(this->_start_orientation, this->_end_orientation, AmountOfRotationComplete);

	sData data;
	data.setSubsystem(_subsystem);
	data.setCMD(SET_ORIENTATION);
	data.setTarget(_target);
	data.setQuatData(qCurrentOrientation);
	pcmdmngr->IssueCommand(data);

	return false;
}

float cRotate::SmootherStep(float startdist, float enddist, float currdist) {
	// Scale, and clamp x to 0..1 range
	currdist = Clamp((currdist - startdist) / (enddist - startdist));

	// Evaluate polynomial
	return currdist * currdist * currdist * (currdist * (currdist * 6 - 15) + 10);
}

float cRotate::Clamp(float x) {
	if (x < 0.0f) { return 0.0f; }
	if (x > 1.0f) { return 1.0f; }
	return x;
}

void cRotate::AddSerialCommand(iCommand* pCommand) { }
void cRotate::AddParallelCommand(iCommand* pCommand) { }
