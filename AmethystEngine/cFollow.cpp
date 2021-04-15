#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#include "cFollow.hpp"
#include "cCommandManager.hpp"

void cFollow::Init(sData data) {
	// check for data state object or curve
	this->_subsystem = data.getSubsystem();
	this->_target = data.getTarget();
	if (data.getSource() != "") { this->_isObject = true; }

	if (this->_isObject) {
		this->_source = data.getSource();
		// needs to get the endposition and startposition
		if (data.getVec4Data()[0] != glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)) { this->_isRelative = false; }
		if (!_isRelative) { this->_start_position = glm::vec3(data.getVec4Data()[0]); }
		else { this->_start_position = glm::vec3(0.0f, 0.0f, 0.0f); }
		this->_end_position = glm::vec3(0.0f, 0.0f, 0.0f);
		this->_max_speed = data.getFloatData()[1];
		this->_target_offset = (glm::vec3)data.getVec4Data()[1];
		this->_inner_radius = data.getFloatData()[2];
		this->_outer_radius = data.getFloatData()[3];
		this->_time = data.getFloatData()[0];
	}
	else {
		if (data.getVec4Data()[0] != glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)) { this->_isRelative = false; }
		if (!this->_isRelative) { this->_start_position = (glm::vec3)data.getVec4Data()[0]; }
		this->_control_point = (glm::vec3)data.getVec4Data()[1];
		this->_current_pos = glm::vec3(0.0f, 0.0f, 0.0f);
		this->_end_position = (glm::vec3)data.getVec4Data()[2];
		this->_curve_percent = 0.0f;
		this->_max_percent = data.getFloatData()[0];
		if (data.isEase()) { this->_easeIn = data.getFloatData()[1]; this->_easeOut = data.getFloatData()[2]; }
		this->_easeInDist = 0.0f;
		this->_easeOutDist = 0.0f;
	}
}

bool cFollow::Update(double deltatime) {
	cCommandManager* pcmdmngr = cCommandManager::GetCommandManager();
	
	if (this->_isObject) {
		if (!this->_UpdateHasBeenCalled) {
			if (this->_isRelative) {
				sData data;
				data.setSubsystem(this->_subsystem);
				data.setCMD(GET_POSITION);
				data.setTarget(this->_source);
				pcmdmngr->IssueCommand(data);
				data.setTarget(this->_target);
				pcmdmngr->IssueCommand(data);

				this->_start_position = (glm::vec3)data.getVec4Data()[0];
				this->_end_position = (glm::vec3)data.getVec4Data()[1];
			}
			this->_UpdateHasBeenCalled = true;
		}



		if (this->_current_time > this->_time)
			return true;
		
		this->_current_time += deltatime;

		sData data;
		data.setSubsystem(_subsystem);
		data.setCMD(GET_POSITION);
		data.setTarget(this->_source);
		pcmdmngr->IssueCommand(data);
		data.setTarget(this->_target);
		pcmdmngr->IssueCommand(data);
		this->_start_position = (glm::vec3)data.getVec4Data()[0];
		this->_end_position = (glm::vec3)data.getVec4Data()[1];


		glm::vec3 targetPos = this->_end_position + this->_target_offset;
		glm::vec3 direction = glm::normalize(targetPos - this->_start_position);
		float distanceToTarget = glm::distance(this->_start_position, targetPos);
		glm::vec3 velocity = direction * this->_max_speed;

		// this would ideally be sent to the object and then let physics update it...
		if (distanceToTarget > this->_outer_radius)
			velocity = direction * this->_max_speed;
		else if (distanceToTarget < this->_inner_radius)
			velocity = glm::vec3(0.0f, 0.0f, 0.0f);
		else {
			float distanceSpeedRatio = 1.0f;
			float distanceBetweenCircles = this->_outer_radius - this->_inner_radius;
			float distanceToInnerCircle = distanceToTarget - this->_inner_radius;
			distanceSpeedRatio = distanceToInnerCircle / distanceBetweenCircles;
			float smoothDistanceSpeedRatio = glm::smoothstep(0.0f, 1.0f, distanceSpeedRatio);
			velocity *= smoothDistanceSpeedRatio;
		}
		glm::vec3 movementThisStep = velocity * (float)deltatime;
		this->_start_position += movementThisStep;

		data.setCMD(SET_POSITION);
		data.setTarget(this->_source);
		data.setVec4Data(this->_start_position);
		pcmdmngr->IssueCommand(data);

		return false;
	}
	else {
		// get the starting stuff that is required
		if (!this->_UpdateHasBeenCalled) {
			if (this->_isRelative) {
				sData data;
				data.setSubsystem(this->_subsystem);
				data.setCMD(GET_POSITION);
				data.setTarget(this->_target);
				pcmdmngr->IssueCommand(data);
				this->_start_position = (glm::vec3)data.getVec4Data()[0];
				this->_current_pos = this->_start_position;
			}
			this->_UpdateHasBeenCalled = true;
		}

		if (this->_curve_percent >= 1.0f)
			return true;

		float totalDistance = glm::distance(this->_end_position, this->_start_position);
		float distance = glm::distance(this->_current_pos, this->_start_position);
		float easeModifier = 1.0f;

		this->_easeInDist = (this->_easeIn / 100.0f) * totalDistance;
		this->_easeOutDist = totalDistance - ((this->_easeOut / 100.0f) * totalDistance);

		if (distance < this->_easeInDist && distance != 0)
			easeModifier = glm::clamp((glm::smoothstep(0.0f, this->_easeInDist, distance) * this->_max_percent), 0.0f, 0.9f);
		else if (distance > _easeOutDist&& distance != 0)
			easeModifier = glm::clamp((glm::smoothstep(totalDistance, this->_easeOutDist, distance) * this->_max_percent), 0.1f, 1.0f);

		this->_curve_percent += ((0.01f / this->_max_percent) * easeModifier);

		glm::vec3 q0, q1;
		q0 = glm::mix(this->_start_position, this->_control_point, this->_curve_percent);
		q1 = glm::mix(this->_control_point, this->_end_position, this->_curve_percent);
		this->_current_pos = glm::mix(q0, q1, this->_curve_percent);

		// update position
		sData data;
		data.setSubsystem(this->_subsystem);
		data.setCMD(SET_POSITION);
		data.setTarget(this->_target);
		data.setVec4Data(this->_current_pos);
		pcmdmngr->IssueCommand(data);

		return false;
	}
}

float cFollow::SmootherStep(float startdist, float enddist, float currdist) {
	// Scale, and clamp x to 0..1 range
	currdist = Clamp((currdist - startdist) / (enddist - startdist));

	// Evaluate polynomial
	return currdist * currdist * currdist * (currdist * (currdist * 6 - 15) + 10);
}

float cFollow::Clamp(float x) {
	if (x < 0.0f) { return 0.0f; }
	if (x > 1.0f) { return 1.0f; }
	return x;
}

void cFollow::AddSerialCommand(iCommand* pCommand) { }
void cFollow::AddParallelCommand(iCommand* pCommand) { }
