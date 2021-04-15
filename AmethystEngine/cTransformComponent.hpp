#pragma once

#include "iGameObject.hpp"
#include <glm/gtx/quaternion.hpp>

class cTransformComponent : public iGameObject {
private:
	float scale;
	glm::vec3 position;
	glm::quat orientation;
	glm::mat4 matWorld;

public:
	cTransformComponent() : scale(1.f), position(glm::vec3(0.f)), orientation(glm::quat()), matWorld(glm::mat4(1.f)) {}
	cTransformComponent(float scaleIn, glm::vec3 positionIn, glm::vec3 OrientationIn);
	cTransformComponent(float scaleIn, glm::vec3 positionIn, glm::quat OrientationIn);

	float getScale() { return scale; }
	glm::vec3 getEulerAngle() { return glm::eulerAngles(orientation); }
	glm::quat getQuaternion() { return orientation; }
	glm::vec3 getFrontVector() { return matWorld[2]; } //glm::vec3(0.f, 0.f, 1.f) * this->getQuaternion(); }
	glm::mat4 getWorldMatrix() { return matWorld; }
	void* getParent() { return parentObject; }

	void setPosition(glm::vec3 positionIn) { this->position = positionIn; RecalculateWorldMatrix(); }
	void mutatePosition(glm::vec3 movement) { this->position += movement;  RecalculateWorldMatrix(); }
	void setOrientation(glm::vec3 orientatationIn) { this->orientation = glm::quat(glm::radians(orientatationIn)); RecalculateWorldMatrix(); }
	void setOrientation(glm::quat orientatationIn) { this->orientation = orientatationIn; RecalculateWorldMatrix(); }
	void mutateOrientation(glm::vec3 eulerAngles);
	void setScale(float scaleIn) { this->scale = scaleIn; RecalculateWorldMatrix(); }
	void setMatrix(glm::mat4 transformIn);

	glm::mat4 RecalculateWorldMatrix();

	// Inherited via iGameObject
	virtual unsigned int getUniqueID(void) override;
	virtual glm::vec3 getPosition(void) override;
};