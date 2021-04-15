#pragma once
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "iGameObject.hpp"
#include "iAudioListener.hpp"
#include "cModelObject.hpp"

class cCinemaCamera : public iAudioListener {
private:
	float yaw;
	float pitch;

	float innerSpeedRadius;
	float outerSpeedRadius;
	float maxSpeed;

	glm::vec3 targetPosition;
	glm::vec3 eye;			//defaults  (0.0f, 0.0f, 0.0f)
	glm::vec3 previousPosition;			//defaults  (0.0f, 0.0f, 0.0f)
	glm::vec3 upVector;		//defaults	(0.0f, 1.0f, 0.0f)
	glm::vec3 front;		//defaults  (0.0f, 0.0f, 1.0f)
	glm::vec3 right;
	glm::vec3 up;
	glm::mat4 viewMatrix;

	//Object Camera differences - SET followDistance to 0 for first person
	float followDistance;
	glm::vec3 followOffset;
	iGameObject* followObject;
	iGameObject* lookAtObject;

	//FMOD 
	FMOD_VECTOR fmod_Pos;
	FMOD_VECTOR fmod_Vel;
	FMOD_VECTOR fmod_For;
	FMOD_VECTOR fmod_Up;

public:
	cCinemaCamera();

	glm::vec3 GetEye();
	glm::vec3 GetUpVector();
	glm::vec3 GetFront();
	glm::vec3 GetRight();
	glm::vec3 GetUp();
	glm::mat4 GetViewMatrix();

	void SetUpVector(glm::vec3 upVector);
	void SetObject(iGameObject* newFollowTarget);
	void SetFollowDistance(float distance);
	void MutateFollowDistance(float offset);

	void MoveForBack(float speed);
	void MoveUpDown(float speed);
	void MoveStrafe(float speed);
	void Move();
	void MoveFront();

	void OffsetYaw(float offset);
	void OffsetPitch(float offset);

	//WIKIPEDIA - https://en.wikipedia.org/wiki/Smoothstep
	float smootherstep(float edge0, float edge1, float x);
	float clamp(float x, float lowerlimit, float upperlimit);

	/* iAudioListener */
	FMOD_VECTOR AudLisGetPosition();
	FMOD_VECTOR AudLisGetVelocity();
	FMOD_VECTOR AudLisGetForward();
	FMOD_VECTOR AudLisGetUp();
};