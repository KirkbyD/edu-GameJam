#pragma once
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "iGameObject.hpp"
#include "iAudioListener.hpp"
#include "cModelObject.hpp"


//camera enum
enum CameraType { freeCam, objectCam };

struct sCameraDef {
	sCameraDef()
		: type(CameraType::freeCam)
		, yaw(0.f), pitch(0.f)
		, position(glm::vec3(0.f))
		, front(glm::vec3(0.f, 0.f, 1.f))
		, upVector(glm::vec3(0.f, 1.f, 0.f))
		, viewMatrix(glm::mat4(1.0f))
		, lookAtObject(nullptr)
		//Object Camera
		, followDistance(1.f), followOffset(glm::vec3(0.f))
		, followObject(nullptr)
		//Cinema Camera
		, innerSpeedRadius(1.f), outerSpeedRadius(5.f)
		, maxSpeed(5.f), targetPosition(glm::vec3(0.f))
	{	}

	int type;

	float yaw;
	float pitch;

	float innerSpeedRadius;
	float outerSpeedRadius;
	float maxSpeed;

	glm::vec3 targetPosition;
	glm::vec3 position;
	glm::vec3 upVector;	
	glm::vec3 front;
	glm::mat4 viewMatrix;

	float followDistance;
	glm::vec3 followOffset;
	iGameObject* followObject;
	iGameObject* lookAtObject;
};

class cCamera : public iAudioListener {
private:	
	int type;
	unsigned friendlyID;

	float yaw;
	float pitch;

	float innerSpeedRadius;
	float outerSpeedRadius;
	float maxSpeed;

	glm::vec3 targetPosition;
	glm::vec3 position;			//defaults  (0.0f, 0.0f, 0.0f)
	glm::vec3 previousPosition;			//defaults  (0.0f, 0.0f, 0.0f)
	glm::vec3 upVector;		//defaults	(0.0f, 1.0f, 0.0f)
	glm::vec3 front;		//defaults  (0.0f, 0.0f, 1.0f)
	glm::vec3 previousFront;
	glm::vec3 right;
	glm::vec3 up;
	glm::mat4 viewMatrix;

	float followDistance;
	glm::vec3 followOffset;
	iGameObject* followObject;
	iGameObject* lookAtObject;

	//FMOD 
	FMOD_VECTOR fmod_Pos;
	FMOD_VECTOR fmod_Vel;
	FMOD_VECTOR fmod_For;
	FMOD_VECTOR fmod_Up;

	void RotatePosition();

	//Hacky
	cModelObject* src;
	cModelObject* dst;
	cCamera* mainCam;
	glm::mat4 portalViewMatrix;

public:
	cCamera(unsigned id);
	cCamera(unsigned id, sCameraDef def);

	glm::vec3 GetPosition() { return this->position; }
	glm::vec3 GetUpVector() { return this->upVector; }
	glm::vec3 GetFront() { return this->front; }
	glm::vec3 GetRight() { return this->right; }
	glm::vec3 GetUp() { return this->up; }
	iGameObject* GetFollowObject() { return this->followObject; }
	iGameObject* GetLookAtObject() { return this->lookAtObject; }
	glm::mat4 GetViewMatrix();

	void SetType(int type) { this->type = type; }
	void SetPosition(glm::vec3 Pos) { this->position = Pos; }
	void SetFront(glm::vec3 front) { this->front = front; }
	void SetUpVector(glm::vec3 upVector) { this->upVector = upVector; }
	void SetFollowObject(iGameObject* newFollowTarget) { this->followObject = newFollowTarget; }
	void SetLookAtObject(iGameObject* newLookAtTarget) { this->lookAtObject = newLookAtTarget; }
	void SetFollowDistance(float distance) { this->followDistance = distance; }
	void MutateFollowDistance(float offset);
	void SetFollowOffset(glm::vec3 offset) { this->followOffset = offset; };
	void SetFrontVector(glm::vec3 FrontIn);

	// Hacky
	void SetPortalView(cModelObject* src, cModelObject* dst, cCamera* mainCam);
	void PortalViewUpdate();
	glm::mat4 GetPortalView();
	glm::vec3 GetPortalCameraPos();
	bool isPortal = false;

	void Move();
	void Move(glm::vec3 moveVec);
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