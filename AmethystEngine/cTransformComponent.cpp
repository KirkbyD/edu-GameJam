#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#include <glm\gtx\matrix_decompose.hpp>
#include "cTransformComponent.hpp"


cTransformComponent::cTransformComponent(float scaleIn, glm::vec3 positionIn, glm::vec3 orientationIn)
	:scale(scaleIn), position(positionIn)
{
	orientation = glm::quat(glm::radians(orientationIn));
	this->matWorld = RecalculateWorldMatrix();
}

cTransformComponent::cTransformComponent(float scaleIn, glm::vec3 positionIn, glm::quat orientationIn) 
	:scale(scaleIn), position(positionIn), orientation(orientationIn)
{ 
	this->matWorld = RecalculateWorldMatrix();
}

void cTransformComponent::mutateOrientation(glm::vec3 eulerAngles) {
	eulerAngles += this->getEulerAngle();
	this->orientation = glm::quat(glm::radians(eulerAngles));
	
	RecalculateWorldMatrix();
}

void cTransformComponent::setMatrix(glm::mat4 transformIn) {
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(transformIn, scale, rotation, translation, skew, perspective);

	this->position = translation;
	this->orientation = rotation;
	//this->scale = scale; //?
	this->matWorld = transformIn;
}

glm::mat4 cTransformComponent::RecalculateWorldMatrix() {
	this->matWorld = glm::mat4(1.0f);

	// TRANSLATION
	glm::mat4 matTrans = glm::translate(glm::mat4(1.0f), glm::vec3(this->position.x, this->position.y, this->position.z));

	// ROTATION
	glm::mat4 matRotation = glm::mat4(this->orientation);

	// SCALE
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(this->scale, this->scale, this->scale));

	this->matWorld = this->matWorld * matTrans * matRotation * scale;

	return this->matWorld;
}

unsigned int cTransformComponent::getUniqueID(void) {
	return this->friendlyIDNumber;
}

glm::vec3 cTransformComponent::getPosition(void) {
	return matWorld[3];
}