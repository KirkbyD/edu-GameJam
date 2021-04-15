#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#include "cFreeCamera.hpp"

cFreeCamera::cFreeCamera() {
	this->yaw = 0.0f;
	this->pitch = 0.0f;

	this->eye = glm::vec3(0.0f, 0.0f, 0.0f);
	this->previousPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	this->front = glm::vec3(0.0f, 0.0f, 1.0f);
	this->upVector = glm::vec3(0.0f, 1.0f, 0.0f);
	this->right = glm::normalize(glm::cross(this->upVector, this->front));
	this->up = glm::cross(this->front, this->right);
	this->viewMatrix = glm::mat4(1.0f);

	this->lookAtObject = NULL;

	//FMOD
	this->fmod_For = FMOD_VECTOR{ 0.0f, 0.0f, 0.0f };
	this->fmod_Pos = FMOD_VECTOR{ 0.0f, 0.0f, 0.0f };
	this->fmod_Up = FMOD_VECTOR{ 0.0f, 0.0f, 0.0f };
	this->fmod_Vel = FMOD_VECTOR{ 0.0f, 0.0f, 0.0f };
}

glm::vec3 cFreeCamera::GetEye() {
	return this->eye;
}

glm::vec3 cFreeCamera::GetUpVector() {
	return this->upVector;
}

glm::vec3 cFreeCamera::GetFront() {
	return this->front;
}

void cFreeCamera::SetTarget(iGameObject* newLookAtTarget) {
	this->lookAtObject = newLookAtTarget;
}

iGameObject* cFreeCamera::GetLookAtTarget() {
	return this->lookAtObject;
}

glm::mat4 cFreeCamera::GetViewMatrix() {
	this->viewMatrix = glm::lookAt(this->eye,
		this->eye + this->front,
		this->upVector);

	return this->viewMatrix;
}

void cFreeCamera::SetEye(glm::vec3 location) {
	this->eye = location;
}

void cFreeCamera::SetUpVector(glm::vec3 upVector) {
	this->upVector = upVector;
}

void cFreeCamera::MoveForBack(float speed) {
	this->eye += this->front * speed;
}

void cFreeCamera::MoveUpDown(float speed) {
	this->eye += this->up * speed;
}

void cFreeCamera::MoveStrafe(float speed) {
	this->eye += this->right * speed;
}

void cFreeCamera::MoveFront() {
	if (this->lookAtObject != NULL) {
		this->front = glm::normalize(this->lookAtObject->getPosition() - this->eye);
	}
	else {
		this->front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
		this->front.y = sin(glm::radians(this->pitch));
		this->front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
		this->front = glm::normalize(this->front);
	}
	this->front = glm::normalize(this->front);

	this->right = glm::normalize(glm::cross(this->upVector, this->front));
	this->up = glm::cross(this->front, this->right);
}

void cFreeCamera::OffsetYaw(float offset) {
	//Constrains the yaw to within 360
	this->yaw = glm::mod(this->yaw + offset, 360.0f);
}

void cFreeCamera::OffsetPitch(float offset) {
	this->pitch += offset;
	if (this->pitch > 89.0f)
		this->pitch = 89.0f;
	if (this->pitch < -89.0f)
		this->pitch = -89.0f;
}


FMOD_VECTOR cFreeCamera::AudLisGetPosition() {
	this->fmod_Pos.x = this->eye.x;;
	this->fmod_Pos.y = this->eye.y;
	this->fmod_Pos.z = this->eye.z;
	return this->fmod_Pos;
}

FMOD_VECTOR cFreeCamera::AudLisGetVelocity() {
	glm::vec3 targetVel = this->eye - this->previousPosition;
	this->fmod_Vel.x = targetVel.x;
	this->fmod_Vel.y = targetVel.y;
	this->fmod_Vel.z = targetVel.z;
	return this->fmod_Vel;
}

FMOD_VECTOR cFreeCamera::AudLisGetForward() {
	this->fmod_For.x = -this->front.x;
	this->fmod_For.y = -this->front.y;
	this->fmod_For.z = -this->front.z;
	return this->fmod_For;
}

FMOD_VECTOR cFreeCamera::AudLisGetUp() {
	this->fmod_Up.x = this->up.x;
	this->fmod_Up.y = this->up.y;
	this->fmod_Up.z = this->up.z;
	return this->fmod_Up;
}
