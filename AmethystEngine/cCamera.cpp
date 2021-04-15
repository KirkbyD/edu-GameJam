#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#include "cCamera.hpp"

cCamera::cCamera(unsigned id)
	: friendlyID(id)
	, type(CameraType::freeCam)
	, yaw(0.f), pitch(0.f)
	, position(glm::vec3(0.f)), previousPosition(position)
	, front(glm::vec3(0.f, 0.f, 1.f)), previousFront(front)
	, upVector(glm::vec3(0.f,1.f,0.f))
	, viewMatrix(glm::mat4(1.0f))
	, lookAtObject(nullptr)
	//Object Camera
	, followDistance(1.f), followOffset(glm::vec3(0.f))
	, followObject(nullptr)
	//Cinema Camera
	, innerSpeedRadius(1.f), outerSpeedRadius(70.f)
	, maxSpeed(30.f), targetPosition(glm::vec3(0.f))
	//FMOD
	, fmod_Pos(FMOD_VECTOR{ 0.0f, 0.0f, 0.0f }), fmod_For(FMOD_VECTOR{ 0.0f, 0.0f, 0.0f })
	, fmod_Up(FMOD_VECTOR{ 0.0f, 0.0f, 0.0f }), fmod_Vel(FMOD_VECTOR{ 0.0f, 0.0f, 0.0f })
{
	//More Complicated Sets
	this->right = glm::normalize(glm::cross(this->upVector, this->front));
	this->up = glm::cross(this->front, this->right);
}

cCamera::cCamera(unsigned id, sCameraDef def)
	: friendlyID(id)
	, type(def.type)
	, yaw(def.yaw), pitch(def.pitch)
	, position(def.position), previousPosition(position)
	, front(def.front), previousFront(front)
	, upVector(def.upVector)
	, viewMatrix(def.viewMatrix)
	, lookAtObject(def.lookAtObject)
	//Object Camera
	, followDistance(def.followDistance), followOffset(def.followOffset)
	, followObject(def.followObject)
	//Cinema Camera
	, innerSpeedRadius(def.innerSpeedRadius), outerSpeedRadius(def.outerSpeedRadius)
	, maxSpeed(def.maxSpeed), targetPosition(def.targetPosition)
	//FMOD
	, fmod_Pos(FMOD_VECTOR{ 0.0f, 0.0f, 0.0f }), fmod_For(FMOD_VECTOR{ 0.0f, 0.0f, 0.0f })
	, fmod_Up(FMOD_VECTOR{ 0.0f, 0.0f, 0.0f }), fmod_Vel(FMOD_VECTOR{ 0.0f, 0.0f, 0.0f })
{
	//More Complicated Sets
	this->right = glm::normalize(glm::cross(this->upVector, this->front));
	this->up = glm::cross(this->front, this->right);
}

glm::mat4 cCamera::GetViewMatrix() {
	this->viewMatrix = glm::lookAt(this->position,
		this->position + this->front,
		this->upVector);

	return this->viewMatrix;
}

void cCamera::MutateFollowDistance(float offset) {
	this->followDistance += 0.1 * offset;
	if (this->followDistance < 0.1f)
		this->followDistance = 0.1f;
	else if (this->followDistance > 5.f)
		this->followDistance = 5.f;
}

	void cCamera::SetFrontVector(glm::vec3 FrontIn) {
		glm::vec3 direction = glm::normalize(FrontIn - position);
		pitch = asin(direction.y);
		yaw = atan2(direction.x, direction.z);
	}

void cCamera::Move() {
	if (this->followObject == nullptr)
		return;

	this->previousPosition = this->position;
	this->targetPosition = this->followObject->getPosition();
	this->targetPosition += (followDistance * followOffset) * -this->front;

	//find distance between position and target
	glm::vec3 direction = this->targetPosition - this->position;
	float distance = glm::length(direction);

	//move position based on distance
	float move = smootherstep(this->innerSpeedRadius, this->outerSpeedRadius, distance);

	if (direction.x == 0 && direction.y == 0 && direction.z == 0)
		return;

	this->position += glm::normalize(direction) * move * this->maxSpeed;
}

void cCamera::Move(glm::vec3 moveVec) {
	this->position += moveVec.x * this->right;
	this->position += moveVec.y * this->up;
	this->position += moveVec.z * this->front;
}

//WIKIPEDIA - https://en.wikipedia.org/wiki/Smoothstep
float cCamera::smootherstep(float edge0, float edge1, float x) {
	// Scale, and clamp x to 0..1 range
	x = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
	// Evaluate polynomial
	return x * x * x * (x * (x * 6 - 15) + 10);
}

float cCamera::clamp(float x, float lowerlimit, float upperlimit) {
	if (x < lowerlimit)
		x = lowerlimit;
	if (x > upperlimit)
		x = upperlimit;
	return x;
}

void cCamera::MoveFront() {
	// Always look at 0 for Physics project
	/*glm::vec3 tmp = this->lookAtObject->getPosition();
	tmp.y = 0.f;
	this->front.y = glm::normalize(tmp - this->position).y;*/

	this->previousFront = this->front;

	if (this->lookAtObject != NULL) {
		this->front = glm::normalize(this->lookAtObject->getPosition() - this->position);
	}
	else {
		this->front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
		this->front.y = sin(glm::radians(this->pitch));
		this->front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
	}
	this->front = glm::normalize(this->front);
	this->right = glm::normalize(glm::cross(this->upVector, this->front));
	this->up = glm::cross(this->front, this->right);

	if (this->followObject == nullptr)
		RotatePosition();
	return;
}


// Used to rotate the camera around view object without invoking smootherstep.
// Laurent Couvidou
	// at https://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion
inline void rotate_vector_by_quaternion(const glm::vec3& v, const glm::quat& q, glm::vec3& vprime) {
	// Extract the vector part of the quaternion
	glm::vec3 u(q.x, q.y, q.z);

	// Extract the scalar part of the quaternion
	float s = q.w;

	// Do the math
	vprime = 2.0f * dot(u, v) * u
		+ (s * s - dot(u, u)) * v
		+ 2.0f * s * cross(u, v);
}

void cCamera::RotatePosition() {
	if (this->followObject == nullptr)
	return;

	this->previousPosition = this->position;
	glm::vec3 newPos = this->position - this->followObject->getPosition();

	glm::quat orientation = glm::quat(glm::radians(glm::vec3(this->yaw, this->pitch, 0.f)));

	rotate_vector_by_quaternion(newPos, glm::quat(glm::radians(glm::vec3(this->yaw, this->pitch, 0.f))), newPos);
	this->position = newPos;
}

void cCamera::OffsetYaw(float offset) {
	this->yaw = glm::mod(this->yaw + offset, 360.0f);
}

void cCamera::OffsetPitch(float offset) {
	this->pitch += offset;
	if (this->pitch > 89.0f)
		this->pitch = 89.0f;
	if (this->pitch < -89.0f)
		this->pitch = -89.0f;
}

void cCamera::SetPortalView(cModelObject* src, cModelObject* dst, cCamera* mainCam)
{
	this->src = src;
	this->dst = dst;
	this->mainCam = mainCam;

	this->isPortal = true;
}

void cCamera::PortalViewUpdate()
{
	glm::mat4 orig_view = mainCam->GetViewMatrix();
	src->recalculateWorldMatrix();
	glm::mat4 src_transform = src->matWorld;
	dst->recalculateWorldMatrix();
	glm::mat4 dst_transform = dst->matWorld;

	glm::mat4 mv = orig_view * src_transform;
	glm::mat4 portal_cam =
		// 3. transformation from source portal to the camera - it's the
		//    first portal's ModelView matrix:
		mv
		// 2. object is front-facing, the camera is facing the other way:
		* glm::rotate(glm::mat4(1.0), glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0))
		// 1. go the destination portal; using inverse, because camera
		//    transformations are reversed compared to object
		//    transformations:
		* glm::inverse(dst_transform)
		;
	portalViewMatrix = portal_cam;
}

glm::mat4 cCamera::GetPortalView()
{
	return portalViewMatrix;
}

glm::vec3 cCamera::GetPortalCameraPos()
{
	glm::mat4 inv_view = glm::inverse(portalViewMatrix);
	return glm::vec3(inv_view[3][0], inv_view[3][1], inv_view[3][2]);
}

#pragma region FMOD
FMOD_VECTOR cCamera::AudLisGetPosition() {
	glm::vec3 targetPos;
	if (this->followObject == nullptr) 
		targetPos = this->position;
	else 
		targetPos = this->followObject->getPosition();
	this->fmod_Pos.x = targetPos.x;
	this->fmod_Pos.y = targetPos.y;
	this->fmod_Pos.z = targetPos.z;
	return this->fmod_Pos;
}

FMOD_VECTOR cCamera::AudLisGetVelocity() {
	glm::vec3 targetVel = this->position - this->previousPosition;
	this->fmod_Vel.x = targetVel.x;
	this->fmod_Vel.y = targetVel.y;
	this->fmod_Vel.z = targetVel.z;
	return this->fmod_Vel;
}

FMOD_VECTOR cCamera::AudLisGetForward() {
	this->fmod_For.x = -this->front.x;
	this->fmod_For.y = -this->front.y;
	this->fmod_For.z = -this->front.z;
	return this->fmod_For;
}

FMOD_VECTOR cCamera::AudLisGetUp() {
	this->fmod_Up.x = this->up.x;
	this->fmod_Up.y = this->up.y;
	this->fmod_Up.z = this->up.z;
	return this->fmod_Up;
}
#pragma endregion