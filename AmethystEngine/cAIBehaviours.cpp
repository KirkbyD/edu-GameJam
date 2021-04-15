#pragma region MEM LEAK CHECK
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif
#pragma endregion

#include <glm/gtc/constants.hpp>
#include <random>
#include "cAIBehaviours.hpp"

void cAIBehaviours::seek(sData& data) {
	glm::vec3 desiredVelocity = glm::vec3(0.f);

	// calculates the desired velocity
	if(data.getTargetGameObj() != nullptr)
		desiredVelocity = data.getTargetGameObj()->getPosition() - data.getSourceGameObj()->getPosition();
	else
		desiredVelocity = data.getTargetPos() - data.getSourceGameObj()->getPosition();

	// get the distance from target
	float distance = glm::length(desiredVelocity);

	desiredVelocity = glm::normalize(desiredVelocity);
	desiredVelocity *= data.GetMaxVelocity();

	// calculate the steering force
	glm::vec3 steeringForce = desiredVelocity - data.getSourceGameObj()->getVelocity();

	// add steering force to current velocity
	//data.getSourceGameObj()->ApplyImpulse();
	data.getSourceGameObj()->MutatePosition(data.getSourceGameObj()->getVelocity() + (steeringForce * data.GetDeltaTime()));
}

// FLOCKING, PATH FOLLOWING CALL
void cAIBehaviours::seek(sData& data, glm::vec3 pos) {
	// calculates the desired velocity
	glm::vec3 desiredVelocity = pos - data.getSourceGameObj()->getPosition();

	// get the distance from target
	float distance = glm::length(desiredVelocity);

	desiredVelocity = glm::normalize(desiredVelocity);
	desiredVelocity *= data.GetMaxVelocity();

	// calculate the steering force
	glm::vec3 steer = desiredVelocity - data.getSourceGameObj()->getVelocity();

	// add steering force to current velocity
	// data.getSourceGameObj()->ApplyImpulse(steer * data.GetDeltaTime());
	// above is going to be handled by the manager for this specific call
	// use this call with flocking...
	data.SetSteeringForce(data.getSourceGameObj()->getVelocity() + (steer * data.GetDeltaTime()));
}

void cAIBehaviours::pursure(sData& data) {
	// calculate the number of frames we are looking ahead
	glm::vec3 distance = data.getTargetGameObj()->getPosition() - data.getSourceGameObj()->getPosition();
	int T = glm::length(distance) / glm::length(data.GetMaxVelocity());

	//the future target point the vehicle will pursue towards
	glm::vec3 futurePosition = data.getTargetGameObj()->getPosition() + data.getTargetGameObj()->getVelocity() * (float)T;

	// calculates the desired velocity
	glm::vec3 desiredVelocity = futurePosition - data.getSourceGameObj()->getPosition();

	desiredVelocity = glm::normalize(desiredVelocity);
	desiredVelocity *= data.GetMaxVelocity();

	// calculate the steering force
	glm::vec3 steeringForce = desiredVelocity - data.getSourceGameObj()->getVelocity();

	// add steering force to current velocity
	// data.getSourceGameObj()->ApplyImpulse(data.getSourceGameObj()->getVelocity() + (steer * data.GetDeltaTime()));
	data.getSourceGameObj()->MutatePosition(data.getSourceGameObj()->getVelocity() + (steeringForce * data.GetDeltaTime()));
}

void cAIBehaviours::flee(sData& data) {
	// calculates the desired velocity
	glm::vec3 desiredVelocity = data.getSourceGameObj()->getPosition() - data.getTargetGameObj()->getPosition();

	// get the distance from target
	float distance = glm::length(desiredVelocity);

	desiredVelocity = glm::normalize(desiredVelocity);
	desiredVelocity *= data.GetMaxVelocity();

	// calculate the steering force
	glm::vec3 steeringForce = desiredVelocity - data.getSourceGameObj()->getVelocity();

	// add steering force to current velocity
	//data.getSourceGameObj()->ApplyImpulse(data.getSourceGameObj()->getVelocity() + (steer * data.GetDeltaTime()));
	data.getSourceGameObj()->MutatePosition(data.getSourceGameObj()->getVelocity() + (steeringForce * data.GetDeltaTime()));
}

void cAIBehaviours::approach(sData& data) {
	// calculates the desired velocity
	glm::vec3 sourcePos = data.getSourceGameObj()->getPosition();
	glm::vec3 targetPos = glm::vec3(0.f);
	if (data.getTargetGameObj() != nullptr)
		targetPos = data.getTargetGameObj()->getPosition();
	else
		targetPos = data.getTargetPos();

	glm::vec3 desiredVelocity = targetPos - sourcePos;

	// get the distance from target
	float distance = glm::length(desiredVelocity);
	desiredVelocity = glm::normalize(desiredVelocity);

	// is the game object within the radius around the target
	if (distance < data.GetSlowingRadius())
		// game object is approaching the target and slows down
		desiredVelocity = desiredVelocity * data.GetMaxVelocity() * (distance / data.GetSlowingRadius());
	else
		// target is far away from game object
		desiredVelocity *= data.GetMaxVelocity();

	// calculate the steering force
	glm::vec3 steeringForce = desiredVelocity - data.getSourceGameObj()->getVelocity();

	// add steering force to current velocity
	//data.getSourceGameObj()->ApplyImpulse(data.getSourceGameObj()->getVelocity() + (steeringForce * data.GetDeltaTime()));
	data.getSourceGameObj()->MutatePosition(data.getSourceGameObj()->getVelocity() + (steeringForce * data.GetDeltaTime()));
}

bool cAIBehaviours::approach(sData& data, glm::vec3 pos, float slowingRadius) {
	glm::vec3 maxVelocity = glm::vec3(5.f, 0.f, 5.f);
	glm::vec3 desiredVelocity = pos - data.getSourceGameObj()->getPosition();
	glm::vec3 steeringForce = glm::vec3(0.f);
	float distance = glm::length(desiredVelocity);

	if (distance < slowingRadius) {
		desiredVelocity = glm::normalize(desiredVelocity) * maxVelocity * (distance / slowingRadius);
		steeringForce = desiredVelocity - data.getSourceGameObj()->getVelocity();
		//data.getSourceGameObj()->ApplyImpulse(steeringForce);
		data.getSourceGameObj()->MutatePosition(data.getSourceGameObj()->getVelocity() + (steeringForce * data.GetDeltaTime()));
		return true;
	}
	else {
		desiredVelocity = glm::normalize(desiredVelocity) * maxVelocity;
		steeringForce = desiredVelocity - data.getSourceGameObj()->getVelocity();
		//data.getSourceGameObj()->ApplyImpulse(data.getSourceGameObj()->getVelocity() + (steeringForce * data.GetDeltaTime()));
		data.getSourceGameObj()->MutatePosition(data.getSourceGameObj()->getVelocity() + (steeringForce * data.GetDeltaTime()));
		return false;
	}
}

void cAIBehaviours::evade(sData& data) {
	// calculate the number of frames we are looking ahead
	glm::vec3 distance = data.getTargetPos() - data.getSourceGameObj()->getPosition();
	int T = glm::length(distance) / glm::length(data.GetMaxVelocity());

	//the future target point the vehicle will pursue towards
	glm::vec3 futurePosition = data.getTargetPos() + data.getTargetVel() * (float)T;

	// calculates the desired velocity
	glm::vec3 desiredVelocity = data.getSourceGameObj()->getPosition() - futurePosition;

	desiredVelocity = glm::normalize(desiredVelocity);

	desiredVelocity *= data.GetMaxVelocity();

	// calculate the steering force
	glm::vec3 steeringForce = desiredVelocity - data.getSourceGameObj()->getVelocity();

	// add steering force to current velocity
	//data.getSourceGameObj()->ApplyImpulse(data.getSourceGameObj()->getVelocity() + (steer * data.GetDeltaTime()));
	data.getSourceGameObj()->MutatePosition(data.getSourceGameObj()->getVelocity() + (steeringForce * data.GetDeltaTime()));
}

void cAIBehaviours::wander(sData& data) {
	float vehicleOrientation = glm::radians(data.getSourceGameObj()->getEulerAngle().y);

	//calculate the circle's center point
	glm::vec3 circlePoint = data.getSourceGameObj()->getPosition() + glm::vec3(glm::cos(-vehicleOrientation), 0, glm::sin(-vehicleOrientation)) * data.GetDistanceToCircle();

	//calculate a random spot on the circle's circumference
	std::random_device rd;
	std::uniform_real_distribution<float> dist(0.f, glm::pi<float>() * 2);
	float angle = dist(rd);
	
	float x = glm::sin(angle) * data.GetCircleRadius();
	float z = glm::cos(angle) * data.GetCircleRadius();

	//the target point the wandering vehicle will seek towards
	glm::vec3 targetPosition = glm::vec3(circlePoint.x + x, 0.f, circlePoint.z + z);

	/*calculates the desired velocity */
	glm::vec3 desiredVelocity = targetPosition - data.getSourceGameObj()->getPosition();

	desiredVelocity = glm::normalize(desiredVelocity);

	desiredVelocity *= data.GetMaxVelocity();

	/*calculate the steering force */
	glm::vec3 steeringForce = desiredVelocity - data.getSourceGameObj()->getVelocity();

	/* add steering force to current velocity*/
	//data.getSourceGameObj()->ApplyImpulse(data.getSourceGameObj()->getVelocity() + (steer * data.GetDeltaTime()));
	data.getSourceGameObj()->MutatePosition(data.getSourceGameObj()->getVelocity() + (steeringForce * data.GetDeltaTime()));
}

void cAIBehaviours::idle(sData& data) {
	glm::vec3 invVel(0.f);
	invVel = data.getSourceGameObj()->getVelocity();
	invVel *= -1.f;
	data.getSourceGameObj()->ApplyImpulse(invVel);
}

void cAIBehaviours::seperation(sData& data) {	
	glm::vec3 totalFlee = glm::vec3(0.f);
	float separationRadius = 100.f;
	int neighbourCount = 0;

	for (int i = 0; i < data.getEntities().size(); ++i) {
		if (data.getEntities()[i]->HasActor()) {
			float dist = glm::distance(data.getSourceGameObj()->getPosition(), data.getEntities()[i]->getPosition());
			if ((dist > 0) && (dist < separationRadius)) {
				glm::vec3 fleeVector = data.getSourceGameObj()->getPosition() - data.getEntities()[i]->getPosition();
				glm::normalize(fleeVector);
				fleeVector /= dist;						//scale based on distance
				totalFlee += fleeVector;
				++neighbourCount;
			}
		}
	}

	glm::vec3 steerForce = glm::vec3(0.f);
	glm::vec3 desiredVelocity = glm::vec3(0.f);
	if (neighbourCount > 0) {
		desiredVelocity = totalFlee / (float)neighbourCount;

		glm::normalize(desiredVelocity);
		desiredVelocity *= data.GetMaxVelocity();
		steerForce = desiredVelocity - data.getSourceGameObj()->getVelocity();
		steerForce = glm::normalize(steerForce);
		steerForce *= data.GetMaxVelocity();
	}

	steerForce = data.getSourceGameObj()->getVelocity() + (steerForce * data.GetDeltaTime());

	data.SetSteeringForce(steerForce);
}

void cAIBehaviours::alignment(sData& data) {
	glm::vec3 totalVelocity = glm::vec3(0.f);
	float alignmentRadius = 105.0f;
	int neighbourCount = 0;

	for (int i = 0; i < data.getEntities().size(); ++i) {
		if (data.getEntities()[i]->HasActor()) {
			float dist = glm::distance(data.getSourceGameObj()->getPosition(), data.getEntities()[i]->getPosition());
			if ((dist > 0) && (dist < alignmentRadius)) {
				totalVelocity += data.getEntities()[i]->getFrontVector();
				++neighbourCount;
			}
		}
	}

	glm::vec3 steerForce = glm::vec3(0.f);
	glm::vec3 desiredVelocity = glm::vec3(0.f);
	if (neighbourCount > 0) {
		desiredVelocity = totalVelocity / (float)neighbourCount;

		glm::normalize(desiredVelocity);
		desiredVelocity *= data.GetMaxVelocity();
		steerForce = desiredVelocity - data.getSourceGameObj()->getFrontVector();
		steerForce = glm::normalize(steerForce);
	}
	data.SetSteeringForce(steerForce);
}

void cAIBehaviours::cohesion(sData& data) {
	glm::vec3 totalPosition = glm::vec3(0.f);
	float cohesionRadius = 110.0f;
	int neighbourCount = 0;

	for (int i = 0; i < data.getEntities().size(); ++i) {
		if (data.getEntities()[i]->HasActor()) {
			float dist = glm::distance(data.getSourceGameObj()->getPosition(), data.getEntities()[i]->getPosition());
			if ((dist > 0) && (dist < cohesionRadius)) {
				totalPosition += data.getEntities()[i]->getPosition();
				++neighbourCount;
			}
		}
	}
	
	glm::vec3 steerForce = glm::vec3(0.f);
	glm::vec3 target = glm::vec3(0.f);

	if (neighbourCount > 0) {
		target = totalPosition / (float)neighbourCount;

		cAIBehaviours::seek(data, target);
		steerForce = data.GetSteeringForce();
		steerForce = glm::normalize(steerForce);
	}

	data.SetSteeringForce(data.getSourceGameObj()->getVelocity() + (steerForce * data.GetDeltaTime()));
}

void cAIBehaviours::formCircle(sData& data, nAI::formationAnchor anchor) {
	glm::vec3 pos = anchor.centerPos;
	glm::vec3 newPos = glm::vec3(0.f);
	newPos.x = anchor.radius * glm::sin(anchor.circleoffset);
	newPos.z = anchor.radius * glm::cos(anchor.circleoffset);
	pos += newPos;
	data.setTargetPos(pos);
}

void cAIBehaviours::formBox(sData& data, nAI::formationAnchor anchor) {
	int node = data.getSourceGameObj()->GetActor()->_formation_pos;
	int colPos = std::ceil((node - 1) / 4);
	glm::vec3 targetPos = glm::vec3(0.f);

	if (node > anchor.rowdef && (node < (anchor.nodes - anchor.rowdef) + 1)) {
		if (glm::mod((float)node, 2.f) != 0)
			targetPos.x = (-anchor.offset) + anchor.position.x;
		else
			targetPos.x = (2 * anchor.offset) + anchor.position.x;
		targetPos.z = -((node - 3) / 2) * anchor.offset + anchor.position.z;
	}
	else {
		if (node <= anchor.rowdef) {
			if (glm::mod((float)node, 2.f) != 0)
				targetPos.x = -(node / 2) * anchor.offset + anchor.position.x;
			else
				targetPos.x = (node / 2) * anchor.offset + anchor.position.x;
			targetPos.z = anchor.position.z;
		}
		else {
			node = node - (anchor.rowdef * colPos);

			if (glm::mod((float)node, 2.f) != 0)
				targetPos.x = -(node / 2) * anchor.offset + anchor.position.x;
			else
				targetPos.x = (node / 2) * anchor.offset + anchor.position.x;
			targetPos.z = -(3 * anchor.offset) + anchor.position.z;
		}
	}

	targetPos.y = anchor.position.y;

	data.setTargetPos(targetPos);
}

void cAIBehaviours::formLine(sData& data, nAI::formationAnchor anchor) {
	int node = data.getSourceGameObj()->GetActor()->_formation_pos;
	glm::vec3 targetPos = glm::vec3(0.f);

	// check to see if odd or even...
	if (glm::mod((float)node, 2.f) != 0)
		targetPos.x = ((-(node / 2) * anchor.offset)) + (anchor.position.x);
	else
		targetPos.x = ((node / 2) * anchor.offset) + (anchor.position.x);
	targetPos.y = anchor.position.y;
	targetPos.z = anchor.position.z;

	data.setTargetPos(targetPos);
}

void cAIBehaviours::formWedge(sData& data, nAI::formationAnchor anchor) {
	int node = data.getSourceGameObj()->GetActor()->_formation_pos;
	glm::vec3 targetPos = glm::vec3(0.f);

	if (glm::mod((float)node, 2.f) != 0) {
		targetPos.x = (-(node / 2) * anchor.offset) + anchor.position.x;
		targetPos.y = anchor.position.y;
		targetPos.z = (-(node / 2) * anchor.offset) + anchor.position.z;
	}
	else {
		targetPos.x = ((node / 2) * anchor.offset) + anchor.position.x;
		targetPos.y = anchor.position.y;
		targetPos.z = (-(node / 2) * anchor.offset) + anchor.position.z;
	}

	data.setTargetPos(targetPos);
}

void cAIBehaviours::formColumn(sData& data, nAI::formationAnchor anchor) {
	int node = data.getSourceGameObj()->GetActor()->_formation_pos;
	int colPos = std::ceil((node-1) / 4);
	glm::vec3 targetPos = glm::vec3(0.f);

	if (node > anchor.rowdef) node = node - (anchor.rowdef*colPos);

	if (glm::mod((float)node, 2.f) != 0)
		targetPos.x = -(node/2) * anchor.offset + anchor.position.x;
	else
		targetPos.x = (node/2) * anchor.offset + anchor.position.x;

	targetPos.y = anchor.position.y;
	targetPos.z = -colPos * anchor.offset + anchor.position.z;

	data.setTargetPos(targetPos);
}
