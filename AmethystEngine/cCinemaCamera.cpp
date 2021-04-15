#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#include "cCinemaCamera.hpp"

cCinemaCamera::cCinemaCamera() {
	this->yaw = 0.0f;
	this->pitch = 0.0f;
	this->followDistance = 1.0f;

	this->innerSpeedRadius = 0.1f;
	this->outerSpeedRadius = 10.0f;
	this->maxSpeed = 5.0f;

	this->eye = glm::vec3(0.0f, 0.0f, 0.0f);
	this->targetPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	this->previousPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	this->front = glm::vec3(0.0f, 0.0f, 1.0f);
	this->upVector = glm::vec3(0.0f, 1.0f, 0.0f);
	this->right = glm::normalize(glm::cross(this->upVector, this->front));
	this->up = glm::cross(this->front, this->right);
	this->viewMatrix = glm::mat4(1.0f);

	this->followOffset = glm::vec3(50.0f, 60.0f, 50.0f);
	this->followObject = NULL;
	this->lookAtObject = NULL;

	//FMOD
	this->fmod_For = FMOD_VECTOR{ 0.0f, 0.0f, 0.0f };
	this->fmod_Pos = FMOD_VECTOR{ 0.0f, 0.0f, 0.0f };
	this->fmod_Up = FMOD_VECTOR{ 0.0f, 0.0f, 0.0f };
	this->fmod_Vel = FMOD_VECTOR{ 0.0f, 0.0f, 0.0f };
}

glm::vec3 cCinemaCamera::GetEye() {
	return this->eye;
}

glm::vec3 cCinemaCamera::GetUpVector() {
	return this->upVector;
}

glm::vec3 cCinemaCamera::GetFront() {
	return this->front;
}

glm::vec3 cCinemaCamera::GetRight() {
	return this->right;
}

glm::vec3 cCinemaCamera::GetUp() {
	return up;
}

glm::mat4 cCinemaCamera::GetViewMatrix() {
	this->viewMatrix = glm::lookAt(this->eye,
		this->eye + this->front,
		this->upVector);

	return this->viewMatrix;
}

void cCinemaCamera::SetUpVector(glm::vec3 upVector) {
	this->upVector = upVector;
}

void cCinemaCamera::SetObject(iGameObject* newFollowTarget) {
	this->followObject = newFollowTarget;
	this->lookAtObject = newFollowTarget;
}

void cCinemaCamera::SetFollowDistance(float distance) {
	this->followDistance = distance;
}

void cCinemaCamera::MutateFollowDistance(float offset) {
	this->followDistance += 0.1 * offset;
	if (this->followDistance < 0.1f)
		this->followDistance = 0.1f;
	else if (this->followDistance > 5.f)
		this->followDistance = 5.f;
}

void cCinemaCamera::MoveForBack(float speed) {
	this->eye += this->front * speed;
}

void cCinemaCamera::MoveUpDown(float speed) {
	this->eye += this->up * speed;
}

void cCinemaCamera::MoveStrafe(float speed) {
	this->eye += this->right * speed;
}

void cCinemaCamera::Move() {
	this->previousPosition = this->eye;

	this->targetPosition = this->followObject->getPosition();

	//this->targetPosition += this->front * followOffset.x; //-5.0f * followDistance;
	//this->targetPosition += this->up * followOffset.y; //*followDistance;
	//this->targetPosition += this->right * followOffset.z; //*followDistance;

	this->targetPosition += (followDistance * followOffset) * -this->front;

	//find distance between eye and target
	glm::vec3 direction = this->targetPosition - this->eye;
	float distance = glm::length(direction);

	//move eye based on distance
	float move = smootherstep(this->innerSpeedRadius, this->outerSpeedRadius, distance);
	this->eye += glm::normalize(direction) * move * this->maxSpeed;
}

//WIKIPEDIA - https://en.wikipedia.org/wiki/Smoothstep
float cCinemaCamera::smootherstep(float edge0, float edge1, float x) {
	// Scale, and clamp x to 0..1 range
	x = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
	// Evaluate polynomial
	return x * x * x * (x * (x * 6 - 15) + 10);
}

float cCinemaCamera::clamp(float x, float lowerlimit, float upperlimit) {
	if (x < lowerlimit)
		x = lowerlimit;
	if (x > upperlimit)
		x = upperlimit;
	return x;
}

void cCinemaCamera::MoveFront() {
	this->front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
	glm::vec3 tmp = this->lookAtObject->getPosition();
	tmp.y = 0.f;
	this->front.y = glm::normalize(tmp - this->eye).y;
	this->front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));

	/*if (this->lookAtObject != NULL) {
	}*/
	/*else {
		this->front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
	}*/
	this->front = glm::normalize(this->front);
	this->right = glm::normalize(glm::cross(this->upVector, this->front));
	this->up = glm::cross(this->front, this->right);
}

void cCinemaCamera::OffsetYaw(float offset) {
	this->yaw = glm::mod(this->yaw + offset, 360.0f);
}

void cCinemaCamera::OffsetPitch(float offset) {
	this->pitch += offset;
	if (this->pitch > 89.0f)
		this->pitch = 89.0f;
	if (this->pitch < -89.0f)
		this->pitch = -89.0f;
}

FMOD_VECTOR cCinemaCamera::AudLisGetPosition() {
	glm::vec3 targetPos = this->followObject->getPosition();
	this->fmod_Pos.x = targetPos.x;
	this->fmod_Pos.y = targetPos.y;
	this->fmod_Pos.z = targetPos.z;
	return this->fmod_Pos;
}

FMOD_VECTOR cCinemaCamera::AudLisGetVelocity() {
	glm::vec3 targetVel = this->eye - this->previousPosition;
	this->fmod_Vel.x = targetVel.x;
	this->fmod_Vel.y = targetVel.y;
	this->fmod_Vel.z = targetVel.z;
	return this->fmod_Vel;
}

FMOD_VECTOR cCinemaCamera::AudLisGetForward() {
	this->fmod_For.x = -this->front.x;
	this->fmod_For.y = -this->front.y;
	this->fmod_For.z = -this->front.z;
	return this->fmod_For;
}

FMOD_VECTOR cCinemaCamera::AudLisGetUp() {
	this->fmod_Up.x = this->up.x;
	this->fmod_Up.y = this->up.y;
	this->fmod_Up.z = this->up.z;
	return this->fmod_Up;
}
